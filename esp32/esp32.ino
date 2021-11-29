#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "driver/rtc_io.h"
#include <EEPROM.h>
#include <WiFi.h>

#define SERIAL_UART_BAUD 57600
#define DEVICE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DEVICE_CONFIG_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define BT_DEVICE_ID "A500 DF Selector"

#define SET_DF1 "df1"
#define SET_DF0 "df0"

#define SELECTED_DRIVE_DF0 0
#define SELECTED_DRIVE_DF1 1

#define SEL0_IN GPIO_NUM_26
#define SEL1_IN GPIO_NUM_18
#define SEL0_OUT GPIO_NUM_19
#define SEL1_OUT GPIO_NUM_23

#define TIMEOUT_SEC 20
#define TIMEOUT_TIMER_ID 0

BLECharacteristic *bleConfigCharacteristic;
int selectedBootDrive = SELECTED_DRIVE_DF0;
//EEPROM ADDRESS MAP
const int selectedBootDriveEepromAddress = 0;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
bool timeoutExpired = false, bleStarted = false;

void saveIntToEEPROM (int eepromAddress,int value) {  
  EEPROM.put(eepromAddress, value);    
  EEPROM.commit();
}

int loadIntFromEEPROM(int eepromAddress) {
  int readData;
  EEPROM.get(eepromAddress, readData);
  return readData;
}


void bleSendData(String text){
  Serial.println("bleSendData, text = " + text);
  bleConfigCharacteristic->setValue(text.c_str());  
  bleConfigCharacteristic->notify();
}

class MyBLECallbacks: public BLECharacteristicCallbacks {
  
  void onWrite(BLECharacteristic *pCharacteristic) {
   
    String uuidStr = pCharacteristic->getUUID().toString().c_str();
    Serial.println("MyBLECallbacks, uuidStr = " + uuidStr);
    String dataReceived = pCharacteristic->getValue().c_str();    
    Serial.println("MyBLECallbacks, dataReceived = " + dataReceived);    
    if (dataReceived.equals(SET_DF0)) {
      Serial.println("MyBLECallbacks, set df0 as boot drive.");    
      selectedBootDrive = SELECTED_DRIVE_DF0;
      saveIntToEEPROM(selectedBootDriveEepromAddress,selectedBootDrive);
    }
    else if (dataReceived.equals(SET_DF1)) {
      Serial.println("MyBLECallbacks, set df1 as boot drive.");    
      selectedBootDrive = SELECTED_DRIVE_DF1;
      saveIntToEEPROM(selectedBootDriveEepromAddress,selectedBootDrive);
    }
  }
};


class MyBLEServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("MyBLEServerCallbacks, onConnect");      
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("MyBLEServerCallbacks, onDisconnect");            
    }
};

void stopBLE() {
  if (BLEDevice::getInitialized()) {
    Serial.println("stopBLE..");      
  
    BLEDevice::getAdvertising()->stop();    
    BLEDevice::deinit(false);  
  
    Serial.println(" done!");    
  }
}

void startBLE() {
  Serial.println("startBLE..");

  char *bleSSID = BT_DEVICE_ID;  
  BLEDevice::init(bleSSID);  

  BLEServer *bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new MyBLEServerCallbacks());

  BLEService *pService = bleServer->createService(DEVICE_SERVICE_UUID);    

  bleConfigCharacteristic = pService->createCharacteristic(
                                         DEVICE_CONFIG_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  bleConfigCharacteristic->setCallbacks(new MyBLECallbacks());
  bleConfigCharacteristic->setValue(""); 
  
  pService->start();
  Serial.println("startBLE, pService ok");

  BLEAdvertising *pAdvertising = bleServer->getAdvertising();
  pAdvertising->start();

  Serial.println("startBLE done!");
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);  
  timeoutExpired = true;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void enableTimeoutTimer() {
  Serial.println("enableTimeoutTimer..");  
  timer = timerBegin(TIMEOUT_TIMER_ID, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000 * TIMEOUT_SEC, true);
  timerAlarmEnable(timer);
}

void setup() {
  Serial.begin(SERIAL_UART_BAUD);
  Serial.println("Starting...");
  
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  
  EEPROM.begin(512);   
  selectedBootDrive = loadIntFromEEPROM(selectedBootDriveEepromAddress);
  if ((selectedBootDrive != SELECTED_DRIVE_DF0) && (selectedBootDrive != SELECTED_DRIVE_DF1)) {
    selectedBootDrive = SELECTED_DRIVE_DF0;
  }
  Serial.println("selectedBootDrive = " + String(selectedBootDrive));
  
  pinMode(SEL0_OUT, OUTPUT);
  pinMode(SEL1_OUT, OUTPUT);
  pinMode(SEL0_IN, INPUT);
  pinMode(SEL1_IN, INPUT);  

  //enableTimeoutTimer();
}

void loop() {
  if (timeoutExpired) {
    stopBLE();
  }

  if (selectedBootDrive == SELECTED_DRIVE_DF1) {  //works
    digitalWrite(SEL0_OUT, digitalRead(SEL1_IN));     
    digitalWrite(SEL1_OUT, digitalRead(SEL0_IN));
  }
  else if (selectedBootDrive == SELECTED_DRIVE_DF0) {    //works
    digitalWrite(SEL1_OUT, HIGH);
    digitalWrite(SEL0_OUT, digitalRead(SEL0_IN));
  }

  if (!bleStarted) {
    bleStarted = true;
    startBLE();
  }
     
}
