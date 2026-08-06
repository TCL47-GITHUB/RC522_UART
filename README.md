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
```

## API Reference

### Initialization & Config
- `bool begin()`: Initializes the module, sends the command to enter normal mode. Returns `true` if module responds correctly.
- `bool reset()`: Soft resets the RC522 chip.
- `bool antennaOn()` / `bool antennaOff()`: Turns the antenna TX on or off.

### Card Operations
- `bool request(uint8_t reqMode, uint16_t* tagType = nullptr)`: Detects a card in the field. `reqMode` is usually `RC522_REQ_ALL`. Returns `true` if a card is found.
- `bool anticoll(uint8_t* uid)`: Reads the 4-byte UID of the detected card. Returns `true` on success.
- `bool select(uint8_t* uid)`: Selects the card with the matching UID for further operations. Returns `true` on success.
- `bool halt()`: Puts the currently selected card to sleep so it won't be read repeatedly.

### Memory & Security (Mifare Classic 1K/4K)
- `bool authState(uint8_t authMode, uint8_t blockAddr, uint8_t* key, uint8_t* uid)`: Authenticates a block using a 6-byte key (`RC522_AUTH_KEY_A` or `RC522_AUTH_KEY_B`). MUST be called before reading or writing any block.
- `bool readBlock(uint8_t blockAddr, uint8_t* recvData)`: Reads 16 bytes from the specified block into `recvData`.
- `bool writeBlock(uint8_t blockAddr, uint8_t* writeData)`: Writes 16 bytes to the specified block.

### Wallet / Value Operations
- `bool valueOperation(uint8_t op, uint8_t blockAddr, uint32_t value)`: Performs an operation (`RC522_VALUE_RECHARGE` or `RC522_VALUE_DEDUCT`) on a correctly formatted Value Block.
- `bool backupValue(uint8_t sourceBlock, uint8_t destBlock)`: Copies the value from the source block to the destination block.

## Available Examples
1. `ReadUID.ino`: Simple script to detect cards and print their UID.
2. `ReadWriteBlock.ino`: Shows the standard process to request, authenticate, read, and write a data block.
3. `WalletOperation.ino`: Demonstrates how to use PcdValue commands to recharge, deduct, and backup an electronic wallet block on Mifare Classic.
4. `SoftwareSerial.ino`: Shows how to connect the module to an Arduino Uno using `SoftwareSerial` pins 2 and 3.

## License
MIT License. Feel free to use it in your projects.
