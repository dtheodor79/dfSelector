
#define SEL0         5
#define SEL1         6

int sel0Value = 0,sel1Value = 0;

void setup() {
  Serial.begin(57600); 
  delay(10);
  Serial.println();
  Serial.println("Starting...");

}

void loop() {
  int sel0Value = digitalRead(SEL0);
  int sel1Value = digitalRead(SEL1);
  Serial.print("sel0:sel1: " + String(sel0Value) + String(sel1Value) + " ");
  if ((sel0Value == 0) && (sel1Value == 1)) {
    Serial.println("df0 selected!");
  }
  else if ((sel0Value == 1) && (sel1Value == 0)) {
    Serial.println("df1 selected!");
  }
  else {
    Serial.println("invalid selection");
  }
  delay(1000);
}
