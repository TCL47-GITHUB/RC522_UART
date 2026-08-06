# RC522_UART Arduino Library

An Arduino library for the RC522 Mini RFID 13.56MHz module with UART interface.

This library simplifies communication with the RC522 module using the UART protocol documented for the mini version. It works over any `Stream` object, meaning you can use hardware serial ports (`Serial1`, `Serial2`) or `SoftwareSerial`.

## Features
- **Read UIDs**: Easily read the 4-byte serial number of Mifare Classic cards (S50/S70).
- **Authentication**: Supports Key A and Key B authentication.
- **Read & Write Data**: Read and write 16-byte blocks on Mifare Classic cards.
- **Wallet Operations**: Supports increment/decrement operations (PcdValue) and backup operations on Mifare value blocks.

## Wiring (ESP32 Example)
| Module Pin | ESP32 Pin | Note |
|------------|-----------|------|
| VCC        | 5V        | Must be 5V DC |
| GND        | GND       | |
| TX         | RX1       | UART Receive |
| RX         | TX1       | UART Transmit |

*Note: The module uses a fixed baudrate of **115200 bps**.*

## Quick Start
Here is a minimal example to read a UID:

```cpp
#include <Arduino.h>
#include "RC522_UART.h"

// Initialize with Serial1 (e.g. ESP32, Mega)
RC522_UART rfid(&Serial1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); 
  
  if (rfid.begin()) {
    Serial.println("RC522 OK!");
  }
}

void loop() {
  if (rfid.request(RC522_REQ_ALL)) {
    uint8_t uid[4];
    if (rfid.anticoll(uid)) {
      Serial.print("UID: ");
      for (int i = 0; i < 4; i++) {
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      rfid.halt();
      delay(1000);
    }
  }
}
```

## Available Examples
1. `ReadUID.ino`: Simple script to detect cards and print their UID.
2. `ReadWriteBlock.ino`: Shows the standard process to request, authenticate, read, and write a data block.

## License
MIT License. Feel free to use it in your projects.
