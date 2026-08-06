/*
 * RC522_UART Library Example
 * 
 * Tác giả (Author): Trương Công Lý (TCL47)
 * SĐT (Phone): 0392100200
 * Facebook: https://www.facebook.com/truongcongly.fb/
 * X/Twitter: https://x.com/truongcongly47
 * Github: https://github.com/TCL47-GITHUB/RC522_UART
 */
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
  
  // NOTE FOR ESP32 USERS: 
  // Serial1 defaults to pins used by the flash memory!
  // You MUST define the RX and TX pins like this:
  // Serial1.begin(115200, SERIAL_8N1, 16, 17); // RX = 16, TX = 17
  
  // For standard Arduino Mega/Leonardo:
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
