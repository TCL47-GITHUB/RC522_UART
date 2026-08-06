/*
 * RC522_UART Library Example
 * 
 * Tác giả (Author): Trương Công Lý (TCL47)
 * SĐT (Phone): 0392100200
 * Facebook: https://www.facebook.com/truongcongly.fb/
 * X/Twitter: https://x.com/truongcongly47
 * Github: https://github.com/TCL47-GITHUB/RC522_UART
 * 
 * Example: ChangeKey
 * Description: Safely changes Key A of a specific sector by preserving the existing Access Bits.
 * WARNING: Do NOT modify the Access Bits (Bytes 6, 7, 8) unless you know what you are doing. 
 * Doing so incorrectly will permanently lock/brick the sector!
 */
#include <Arduino.h>
#include "RC522_UART.h"

RC522_UART rfid(&Serial1);

// We will change the key of Sector 2 (Trailer block is 2 * 4 + 3 = 11)
uint8_t targetSector = 2;
uint8_t trailerBlock = targetSector * 4 + 3; // Block 11

// The old default key (factory)
uint8_t oldKeyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// The new secret key you want to set
uint8_t newKeyA[6] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); 
  
  if (rfid.begin()) {
    Serial.println("RC522 Ready! Present a card to change Key A of Sector 2...");
  }
}

void loop() {
  if (rfid.request(RC522_REQ_ALL)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      if (rfid.select(uid)) {
        Serial.println("Card detected.");
        
        // 1. Authenticate with OLD key first
        if (rfid.authState(RC522_AUTH_KEY_A, trailerBlock, oldKeyA, uid)) {
          Serial.println("Auth OK with Old Key. Reading Sector Trailer...");
          
          uint8_t trailerData[16];
          // 2. Read the Sector Trailer BEFORE writing!
          // We MUST do this to keep the Access Bits (bytes 6,7,8) unchanged.
          if (rfid.readBlock(trailerBlock, trailerData)) {
            
            // 3. Replace the first 6 bytes with our new Key A
            for (int i = 0; i < 6; i++) {
              trailerData[i] = newKeyA[i];
            }
            
            // 4. Write the modified Sector Trailer back to the card
            if (rfid.writeBlock(trailerBlock, trailerData)) {
              Serial.println("SUCCESS! Key A has been changed.");
              Serial.println("To test it again, you must swap newKeyA and oldKeyA in the code.");
            } else {
              Serial.println("Failed to write new key!");
            }
            
          } else {
            Serial.println("Failed to read Sector Trailer.");
          }
        } else {
          Serial.println("Auth FAILED! The key might have already been changed.");
        }
        
        rfid.halt();
        delay(3000);
      }
    }
  }
}
