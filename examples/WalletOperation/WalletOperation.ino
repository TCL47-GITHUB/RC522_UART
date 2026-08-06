#include <Arduino.h>
#include "RC522_UART.h"

// Initialize with hardware Serial1
RC522_UART rfid(&Serial1);

// Wallet Block settings
// Note: In Mifare Classic, a "value block" has a specific 16-byte format.
// If the block is not formatted as a value block, PcdValue commands may fail.
// This example assumes you have already written a valid value format to this block,
// OR the module automatically handles it (depends on the module's internal firmware).
uint8_t walletBlock = 5; 
uint8_t backupBlock = 6; 
uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  
  // ESP32: define pins if needed -> Serial1.begin(115200, SERIAL_8N1, 16, 17);
  Serial1.begin(115200); 
  delay(1000);
  
  Serial.println("RC522 Wallet Example (Deduct / Recharge)");
  
  if (rfid.begin()) {
    Serial.println("RC522 Initialized successfully.");
  } else {
    Serial.println("Failed to initialize RC522.");
  }
}

void loop() {
  if (rfid.request(RC522_REQ_ALL)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      Serial.println("Card detected!");
      
      if (rfid.select(uid)) {
        // Authenticate the sector containing our wallet block (Block 5 is in Sector 1)
        if (rfid.authState(RC522_AUTH_KEY_A, walletBlock, keyA, uid)) {
          Serial.println("Authentication OK.");
          
          // Example: Recharge (Add value)
          // Value must be formatted as 4-byte LSB. The library handles the LSB conversion internally.
          uint32_t rechargeAmount = 500;
          Serial.print("Recharging: "); Serial.println(rechargeAmount);
          if (rfid.valueOperation(RC522_VALUE_RECHARGE, walletBlock, rechargeAmount)) {
             Serial.println("Recharge SUCCESS!");
          } else {
             Serial.println("Recharge FAILED!");
          }

          delay(500); // Wait a bit
          
          // Example: Deduct (Subtract value)
          uint32_t deductAmount = 150;
          Serial.print("Deducting: "); Serial.println(deductAmount);
          if (rfid.valueOperation(RC522_VALUE_DEDUCT, walletBlock, deductAmount)) {
             Serial.println("Deduction SUCCESS!");
          } else {
             Serial.println("Deduction FAILED!");
          }

          delay(500);

          // Example: Backup wallet value to another block (must be in the same authenticated sector)
          Serial.print("Backing up block "); Serial.print(walletBlock); 
          Serial.print(" to block "); Serial.println(backupBlock);
          
          if (rfid.backupValue(walletBlock, backupBlock)) {
             Serial.println("Backup SUCCESS!");
          } else {
             Serial.println("Backup FAILED!");
          }

        } else {
          Serial.println("Authentication FAILED!");
        }
      }
      
      rfid.halt();
      Serial.println("Done. Card halted.");
      delay(3000); // Wait 3 seconds before next scan
    }
  }
}
