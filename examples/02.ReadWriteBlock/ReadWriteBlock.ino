#include <Arduino.h>
#include "RC522_UART.h"

// Make sure your board has Serial1 available, 
// or use SoftwareSerial for boards like Arduino Uno.
RC522_UART rfid(&Serial1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); 
  delay(1000);
  
  Serial.println("RC522 Read/Write Block Example");
  
  if (rfid.begin()) {
    Serial.println("RC522 Initialized successfully.");
  } else {
    Serial.println("Failed to initialize RC522.");
  }
}

void loop() {
  // Step 1: Request card
  if (rfid.request(RC522_REQ_ALL)) {
    
    // Step 2: Read UID
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      Serial.print("UID detected: ");
      for (int i = 0; i < 4; i++) {
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Step 3: Select card
      if (rfid.select(uid)) {
        
        // Step 4: Authenticate (we will use Block 4, which is in Sector 1)
        uint8_t blockAddr = 4;
        uint8_t key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Default key A
        
        if (rfid.authState(RC522_AUTH_KEY_A, blockAddr, key, uid)) {
          Serial.println("Authentication OK.");
          
          // Step 5: Read Block
          uint8_t dataRead[16];
          if (rfid.readBlock(blockAddr, dataRead)) {
            Serial.print("Data at Block ");
            Serial.print(blockAddr);
            Serial.print(": ");
            for (int i = 0; i < 16; i++) {
              if (dataRead[i] < 0x10) Serial.print("0");
              Serial.print(dataRead[i], HEX);
              Serial.print(" ");
            }
            Serial.println();
            
            // Step 6: Write to Block (Warning: this modifies the card!)
            // We will write a simple string "Hello RC522_UART"
            uint8_t dataWrite[16] = {
              'H','e','l','l','o',' ','R','C','5','2','2','_','U','A','R','T'
            };
            
            if (rfid.writeBlock(blockAddr, dataWrite)) {
              Serial.println("Write OK!");
            } else {
              Serial.println("Write FAILED!");
            }
            
          } else {
            Serial.println("Read FAILED!");
          }
          
        } else {
          Serial.println("Authentication FAILED!");
        }
      }
      
      // Step 7: Halt the card to finish operations safely
      rfid.halt();
      Serial.println("Card halted.");
      delay(2000); // Wait a bit before scanning again
    }
  }
}
