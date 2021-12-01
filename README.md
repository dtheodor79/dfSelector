# Bluetoot DF selector
 The Bluetooth DF (BDF) selector allows users to boot your Amiga 500 from an internal or external drive. Instead of using a mechanical switch or multiple reset sequences to toggle between DF0 and DF1, BDF can be configured wirelessly over a Bluetooth connection. 
 
# Why bother when there are already other DF selectors?
Well, all this started because I don't like modifying my A500's precious case (like installing a switch) 🙂. Also, I find more elegant using wireless commands to toggle between DF0 and DF1, than multiple resets 😎.
 
# Concept
The BDF concept is illustrated below: 

![concept](/images/concept.png)

Pins 13 (sel0) and 14 (sel1) from 8520A (even CIA) are connected to an MCU with wireless connection capability. Users can configure from an exteneral device how these signals are forwarded to the A500 motherboard (MB), in order to select from which floppy drive the computer shall boot. 

# Implementation
The image below shows a BDF 3D model that is implemented with an ESP32 MCU, which enables switch configuration over Bluetooth connections:

![concept](/images/df_selector_3d_model.png)

A voltage convertor between 3.3V and 5V is used to allow communication between the ESP32 (3.3V) and the 8520A (5V). In addition, since the BDF relies above the floppy ribbon cable connector of the A500, it uses long pin headers (such as the one below) to connect to the A500 MB:

![concept](/images/long_pin_headers.jpg)

# How to use the ESP32-based implementation
The current ESP32 firmware advertises a Bluetooth-enabled device with the name "A500 DF Selector". You will need another Bluetooth-enabled device to connect to the BDF and then send raw data for configuring the sel0 and sel1 switching. An easy way is to download a simple Bluetooth app to your smartphone that allows sending raw data to other devices. 

## Toggle DF1 as bootable:

* Make sure that there is no disk in DF0.
* Power-up the A500 and wait for few secs (e.g. 10 would be enough).
* Scan for the "A500 DF Selector" device, and when found, connect to it.
* Send "df1".
* Power-down the A500 for a few secs.
* Power-up again, and now the A500 should boot from DF1.

## Toggle DF0 as bootable:

* Make sure that there is no disk (or adf image that will boot) in DF1.
* Power-up the A500 and wait for few secs (e.g. 10 would be enough).
* Scan for the "A500 DF Selector" device, and when found, connect to it.
* Send "df0".
* Power-down the A500 for a few secs.
* Power-up again, and now the A500 should boot from DF0.

# Important notes
- All tests were done with a modified PC-floppy as DF0 and a Gotek emulator as DF1.
- When DF0 is selected, the current firmware simply forwards sel0 and sel1 from 8520A to the A500 MB.
- When DF1 is selected, the current firmware forces sel0 to HIGH, i.e. internal drive is disabled.
- Feel free to change the firmware
- During tests, there were cases where when DF0 was toggled, the first time disk read was failing. A simple reset fixed it 😉.

# Pictures of the ESP32 implementation
Here are a few images of the final prototype on my A500:

![concept](/images/image1.jpg)
![concept](/images/image2.jpg)
![concept](/images/image3.jpg)
![concept](/images/image4.jpg)

Hope you find it useful!! 😎
