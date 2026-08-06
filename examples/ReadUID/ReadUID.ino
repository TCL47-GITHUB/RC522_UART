#include <Arduino.h>
#include "RC522_UART.h"

// We will use Serial1 on ESP32 or Mega.
// For Arduino Uno, you might want to use SoftwareSerial.
// e.g. 
// #include <SoftwareSerial.h>
// SoftwareSerial mySerial(2, 3); // RX, TX
// RC522_UART rfid(&mySerial);

// Here we assume a board with hardware Serial1
RC522_UART rfid(&Serial1);

void setup() {
  Serial.begin(115200);
  
  // Initialize Serial1 for RC522 (Baudrate MUST be 115200)
  Serial1.begin(115200); 
  
  // Wait for Serial ports to be ready
  delay(1000);
  
  Serial.println("Initializing RC522...");
  
  // Begin the RFID module (enters normal mode)
  if (rfid.begin()) {
    Serial.println("RC522 Initialization OK!");
  } else {
    Serial.println("RC522 Initialization FAILED! Check wiring.");
  }
}

void loop() {
  uint16_t tagType;
  
  // Look for any cards in the field
  if (rfid.request(RC522_REQ_ALL, &tagType)) {
    Serial.print("Card detected! Type: 0x");
    Serial.println(tagType, HEX);
    
    // Read the UID
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      Serial.print("Card UID: ");
      for (int i = 0; i < 4; i++) {
        if (uid[i] < 0x10) Serial.print("0");
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Halt the card so it doesn't get read repeatedly very fast
      rfid.halt();
      delay(1000);
    }
  }
  
  delay(100);
}
