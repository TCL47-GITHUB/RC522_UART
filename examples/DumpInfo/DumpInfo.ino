/*
 * RC522_UART Library Example
 * 
 * Tác giả (Author): Trương Công Lý (TCL47)
 * SĐT (Phone): 0392100200
 * Facebook: https://www.facebook.com/truongcongly.fb/
 * X/Twitter: https://x.com/truongcongly47
 * Github: https://github.com/TCL47-GITHUB/RC522_UART
 * 
 * Example: DumpInfo
 * Description: Reads and prints the entire contents of a Mifare Classic 1K card (all 64 blocks).
 */
#include <Arduino.h>
#include "RC522_UART.h"

RC522_UART rfid(&Serial1);

// Default Key A for fresh cards
uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); // TX=17, RX=16 on ESP32 default for Serial1
  
  Serial.println("Initializing RC522...");
  if (rfid.begin()) {
    Serial.println("RC522 Ready! Present a Mifare Classic 1K card...");
  } else {
    Serial.println("Failed to initialize RC522.");
  }
}

void loop() {
  if (rfid.request(RC522_REQ_ALL)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      if (rfid.select(uid)) {
        Serial.println("\n-----------------------------------------");
        Serial.print("Card detected! UID: ");
        for(int i=0; i<4; i++) {
          if(uid[i] < 0x10) Serial.print("0");
          Serial.print(uid[i], HEX);
          Serial.print(" ");
        }
        Serial.println("\nStarting memory dump (16 Sectors / 64 Blocks)...");

        // Loop through all 16 sectors (Sector 15 down to 0)
        for (int sector = 15; sector >= 0; sector--) {
          Serial.print("Sector "); Serial.println(sector);
          
          // Authenticate the sector (we use the sector trailer block)
          uint8_t trailerBlock = sector * 4 + 3;
          if (rfid.authState(RC522_AUTH_KEY_A, trailerBlock, keyA, uid)) {
            // Read all 4 blocks in this sector (from Block 3 down to Block 0)
            for (int blockOffset = 3; blockOffset >= 0; blockOffset--) {
              uint8_t currentBlock = sector * 4 + blockOffset;
              uint8_t data[16];
              
              if (rfid.readBlock(currentBlock, data)) {
                Serial.print("  Block ");
                if (currentBlock < 10) Serial.print("0");
                Serial.print(currentBlock);
                Serial.print(" | ");
                
                // Print data in HEX
                for (int i = 0; i < 16; i++) {
                  if (data[i] < 0x10) Serial.print("0");
                  Serial.print(data[i], HEX);
                  Serial.print(" ");
                }
                Serial.println();
              } else {
                Serial.print("  Block "); Serial.print(currentBlock);
                Serial.println(" | Read FAILED");
              }
            }
          } else {
            Serial.println("  Authentication FAILED (Wrong Key?)");
          }
          Serial.println(""); // Empty line between sectors
        }
        
        Serial.println("Dump complete.");
        Serial.println("-----------------------------------------\n");
        
        rfid.halt();
        delay(2000); // Wait 2 seconds before next scan
      }
    }
  }
}
