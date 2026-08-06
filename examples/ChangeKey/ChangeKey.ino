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
 * Description: Dynamically detects the current Key A from a list of known keys, 
 * then asks the user to input a NEW Key A via the Serial Monitor.
 * WARNING: Do NOT modify the Access Bits (Bytes 6, 7, 8) unless you know what you are doing. 
 */
#include <Arduino.h>
#include "RC522_UART.h"

RC522_UART rfid(&Serial1);

// We will change the key of Sector 2 (Trailer block is 2 * 4 + 3 = 11)
uint8_t targetSector = 2;
uint8_t trailerBlock = targetSector * 4 + 3; // Block 11

// List of common known keys (Factory default, NDEF, etc.)
const int numKnownKeys = 4;
uint8_t knownKeys[numKnownKeys][6] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Factory Default
  {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}, // NDEF Format
  {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6}, // Custom Test Key
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // All Zero
};

void printKey(uint8_t* key) {
  for (int i = 0; i < 6; i++) {
    if (key[i] < 0x10) Serial.print("0");
    Serial.print(key[i], HEX);
    Serial.print(" ");
  }
}

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
        Serial.println("\n--- Card detected! ---");
        
        // 1. Try to guess the current key
        int matchedKeyIndex = -1;
        for (int i = 0; i < numKnownKeys; i++) {
          if (rfid.authState(RC522_AUTH_KEY_A, trailerBlock, knownKeys[i], uid)) {
            matchedKeyIndex = i;
            break;
          }
        }
        
        if (matchedKeyIndex != -1) {
          Serial.print("SUCCESS! Found current Key A: ");
          printKey(knownKeys[matchedKeyIndex]);
          Serial.println();
          
          // 2. Read Sector Trailer (to preserve access bits)
          uint8_t trailerData[16];
          if (rfid.readBlock(trailerBlock, trailerData)) {
            
            // 3. Ask user for new key
            Serial.println(">>> Please type the NEW 6-byte key in HEX (e.g. 1A 2B 3C 4D 5E 6F) into Serial Monitor:");
            
            // Clear Serial buffer
            while(Serial.available()) Serial.read();
            
            // Wait for user input
            while(Serial.available() < 12) { // Need at least 12 hex chars + spaces
              delay(10);
            }
            
            // Parse user input
            uint8_t newKey[6];
            int keyIndex = 0;
            String inputBuffer = "";
            while (Serial.available() && keyIndex < 6) {
              char c = Serial.read();
              if (c == ' ' || c == '\n' || c == '\r') continue;
              inputBuffer += c;
              
              if (inputBuffer.length() == 2) {
                newKey[keyIndex] = (uint8_t) strtol(inputBuffer.c_str(), NULL, 16);
                inputBuffer = "";
                keyIndex++;
              }
            }
            
            Serial.print("You entered new Key A: ");
            printKey(newKey);
            Serial.println();
            
            // 4. Update Key A in trailer data
            for (int i = 0; i < 6; i++) {
              trailerData[i] = newKey[i];
            }
            
            // 5. Write back to card
            if (rfid.writeBlock(trailerBlock, trailerData)) {
              Serial.println(">>> KEY CHANGED SUCCESSFULLY! <<<");
              Serial.println("Please update the 'knownKeys' array in code if you want to detect this key next time.");
            } else {
              Serial.println("Write Failed!");
            }
            
          } else {
            Serial.println("Failed to read Sector Trailer.");
          }
        } else {
          Serial.println("Auth FAILED! None of the known keys worked.");
          Serial.println("Please add your card's key to the 'knownKeys' array.");
        }
        
        rfid.halt();
        delay(2000);
        
        // Clear serial buffer before next card
        while(Serial.available()) Serial.read();
        Serial.println("\nReady for next card...");
      }
    }
  }
}
