#include <Arduino.h>
#include <SoftwareSerial.h>
#include "RC522_UART.h"

// For Arduino Uno, Nano, or any board without extra hardware serial ports.
// We define a SoftwareSerial port on digital pins 2 and 3.
// Pin 2: RX (Connect to TX of RC522)
// Pin 3: TX (Connect to RX of RC522)
SoftwareSerial rfidSerial(2, 3);

// Pass the SoftwareSerial stream to the library
RC522_UART rfid(&rfidSerial);

void setup() {
  // Serial monitor for debugging
  Serial.begin(115200);
  
  // SoftwareSerial port for the RC522 module
  // IMPORTANT: The RC522 module defaults to 115200 bps. 
  // SoftwareSerial at 115200 on an Arduino Uno can sometimes be unstable.
  // If you experience issues, it's a limitation of SoftwareSerial at high speeds.
  rfidSerial.begin(115200); 
  
  delay(1000);
  Serial.println("Initializing RC522 via SoftwareSerial...");
  
  if (rfid.begin()) {
    Serial.println("RC522 Initialization OK!");
  } else {
    Serial.println("RC522 Initialization FAILED! Check wiring and SoftwareSerial stability.");
  }
}

void loop() {
  uint16_t tagType;
  
  if (rfid.request(RC522_REQ_ALL, &tagType)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      Serial.print("Card UID: ");
      for (int i = 0; i < 4; i++) {
        if (uid[i] < 0x10) Serial.print("0");
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      rfid.halt();
      delay(1000);
    }
  }
}
