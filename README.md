# RC522_UART Arduino Library

[🇻🇳 Đọc bằng Tiếng Việt (Vietnamese)](README_vn.md)

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

## Constants & Definitions

The library uses the following constants for its parameters:

**Request Modes (`reqMode`)**
- `RC522_REQ_ALL` (0x52): Finds all cards in the antenna area, including those that were put to sleep (halted).
- `RC522_REQ_NOT_HALT` (0x26): Only finds cards that are awake (not halted).

**Authentication Modes (`authMode`)**
- `RC522_AUTH_KEY_A` (0x60): Authenticate using Key A.
- `RC522_AUTH_KEY_B` (0x61): Authenticate using Key B.

**Value Operations (`op`)**
- `RC522_VALUE_RECHARGE` (0xC1): Adds the specified value to the Wallet Block.
- `RC522_VALUE_DEDUCT` (0xC0): Subtracts the specified value from the Wallet Block.

## API Reference

### Initialization & Config
- `bool begin()`: Initializes the module, sends the command to enter normal mode. Returns `true` if module responds correctly.
  ```cpp
  if (rfid.begin()) {
    Serial.println("RC522 Started!");
  }
  ```
- `bool reset()`: Soft resets the RC522 chip.
  ```cpp
  rfid.reset();
  ```
- `bool antennaOn()` / `bool antennaOff()`: Turns the antenna TX on or off.
  ```cpp
  rfid.antennaOff(); // Disable field
  delay(100);
  rfid.antennaOn();  // Enable field
  ```

### Card Operations
- `bool request(uint8_t reqMode, uint16_t* tagType = nullptr)`: Detects a card in the field. `reqMode` can be `RC522_REQ_ALL` (find all cards) or `RC522_REQ_IDLE` (find only cards not in sleep state). Returns `true` if a card is found.
  ```cpp
  uint16_t tagType;
  if (rfid.request(RC522_REQ_ALL, &tagType)) {
    Serial.println("Card detected!");
  }
  ```
- `bool anticoll(uint8_t* uid)`: Reads the 4-byte UID of the detected card. Returns `true` on success.
  ```cpp
  uint8_t uid[4];
  if (rfid.anticoll(uid)) {
    Serial.print("UID: ");
    Serial.println(uid[0], HEX);
  }
  ```
- `bool select(uint8_t* uid)`: Selects the card with the matching UID for further operations. Returns `true` on success.
  ```cpp
  if (rfid.select(uid)) {
    Serial.println("Card selected for read/write.");
  }
  ```
- `bool halt()`: Puts the currently selected card to sleep so it won't be read repeatedly.
  ```cpp
  rfid.halt(); // Call this after you are done with the card
  ```

### Memory & Security (Mifare Classic 1K/4K)
- `bool authState(uint8_t authMode, uint8_t blockAddr, uint8_t* key, uint8_t* uid)`: Authenticates a block using a 6-byte key. `authMode` can be `RC522_AUTH_KEY_A` or `RC522_AUTH_KEY_B`. MUST be called before reading or writing any block.
  ```cpp
  uint8_t key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if (rfid.authState(RC522_AUTH_KEY_A, 4, key, uid)) {
    Serial.println("Block 4 Authenticated!");
  }
  ```
- `bool readBlock(uint8_t blockAddr, uint8_t* recvData)`: Reads 16 bytes from the specified block into `recvData`.
  ```cpp
  uint8_t data[16];
  if (rfid.readBlock(4, data)) {
    Serial.print("Data block 4: ");
    Serial.println(data[0]);
  }
  ```
- `bool writeBlock(uint8_t blockAddr, uint8_t* writeData)`: Writes 16 bytes to the specified block.
  ```cpp
  uint8_t data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  if (rfid.writeBlock(4, data)) {
    Serial.println("Write successful!");
  }
  ```

### Wallet / Value Operations
- `bool valueOperation(uint8_t op, uint8_t blockAddr, uint32_t value)`: Performs a math operation on a formatted Value Block. `op` can be `RC522_VALUE_RECHARGE` (Add) or `RC522_VALUE_DEDUCT` (Subtract).
  ```cpp
  // Add 500 to Wallet Block 5
  if (rfid.valueOperation(RC522_VALUE_RECHARGE, 5, 500)) {
    Serial.println("Recharged 500!");
  }
  ```
- `bool backupValue(uint8_t sourceBlock, uint8_t destBlock)`: Copies the value from the source block to the destination block.
  ```cpp
  // Backup value from block 5 to block 6
  if (rfid.backupValue(5, 6)) {
    Serial.println("Backup successful!");
  }
  ```

## Available Examples
1. `ReadUID.ino`: Simple script to detect cards and print their UID.
2. `ReadWriteBlock.ino`: Shows the standard process to request, authenticate, read, and write a data block.
3. `WalletOperation.ino`: Demonstrates how to use PcdValue commands to recharge, deduct, and backup an electronic wallet block on Mifare Classic.
4. `FormatValueBlock.ino`: Shows how to format a blank block into the strict 16-byte Value Block format required for WalletOperation.
5. `SoftwareSerial.ino`: Shows how to connect the module to an Arduino Uno using `SoftwareSerial` pins 2 and 3.
6. `DumpInfo.ino`: Reads and prints the entire contents of a Mifare Classic 1K card (all 16 sectors, 64 blocks) to the serial monitor. Useful for debugging.
7. `ChangeKey.ino`: Demonstrates how to safely change Key A of a sector by reading the Sector Trailer first to preserve access bits, preventing accidental bricking of the card.

## License
MIT License. Feel free to use it in your projects.
