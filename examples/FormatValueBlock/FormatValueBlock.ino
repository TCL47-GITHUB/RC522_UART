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

RC522_UART rfid(&Serial1);

uint8_t walletBlock = 5; 
uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  
  // ESP32: Serial1.begin(115200, SERIAL_8N1, 16, 17);
  Serial1.begin(115200); 
  delay(1000);
  
  Serial.println("RC522 Format Value Block Example");
  
  if (rfid.begin()) {
    Serial.println("RC522 Initialized successfully.");
  } else {
    Serial.println("Failed to initialize.");
  }
}

void loop() {
  if (rfid.request(RC522_REQ_ALL)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      if (rfid.select(uid)) {
        if (rfid.authState(RC522_AUTH_KEY_A, walletBlock, keyA, uid)) {
          
          // To use PcdValue (Recharge/Deduct), the block MUST be formatted as a Value Block.
          // The format is 16 bytes:
          // [Value_LSB, Value_Mid1, Value_Mid2, Value_MSB]
          // [~Value_LSB, ~Value_Mid1, ~Value_Mid2, ~Value_MSB] (Inverted)
          // [Value_LSB, Value_Mid1, Value_Mid2, Value_MSB]
          // [Addr, ~Addr, Addr, ~Addr]
          
          // Let's format it with an initial value of 1000 (0x000003E8)
          uint32_t initialValue = 1000;
          uint8_t v1 = (initialValue & 0xFF);
          uint8_t v2 = ((initialValue >> 8) & 0xFF);
          uint8_t v3 = ((initialValue >> 16) & 0xFF);
          uint8_t v4 = ((initialValue >> 24) & 0xFF);
          
          uint8_t addr = walletBlock;
          
          uint8_t valueBlockData[16] = {
            v1, v2, v3, v4,
            (uint8_t)~v1, (uint8_t)~v2, (uint8_t)~v3, (uint8_t)~v4,
            v1, v2, v3, v4,
            addr, (uint8_t)~addr, addr, (uint8_t)~addr
          };
          
          if (rfid.writeBlock(walletBlock, valueBlockData)) {
            Serial.println("FORMAT SUCCESS! Block is now a Wallet.");
          } else {
            Serial.println("FORMAT FAILED!");
          }
        } else {
          Serial.println("Authentication FAILED! Check if card is Mifare 1K and key is correct.");
        }
      }
      rfid.halt();
      delay(3000);
    }
  }
}
