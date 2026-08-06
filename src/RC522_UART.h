#ifndef RC522_UART_H
#define RC522_UART_H

#include <Arduino.h>

// MFRC522 UART Command Bytes
#define RC522_CMD_RESET       0x00
#define RC522_CMD_ANT_ON      0x01
#define RC522_CMD_ANT_OFF     0x02
#define RC522_CMD_REQUEST     0x03
#define RC522_CMD_ANTICOLL    0x04
#define RC522_CMD_SELECT      0x05
#define RC522_CMD_AUTH        0x06
#define RC522_CMD_READ        0x07
#define RC522_CMD_WRITE       0x08
#define RC522_CMD_VALUE       0x09
#define RC522_CMD_BAK_VALUE   0x0A
#define RC522_CMD_HALT        0x0B

// Parameters
#define RC522_REQ_ALL         0x52
#define RC522_REQ_NOT_HALT    0x26

#define RC522_AUTH_KEY_A      0x60
#define RC522_AUTH_KEY_B      0x61

#define RC522_VALUE_DEDUCT    0xC0
#define RC522_VALUE_RECHARGE  0xC1

// Packet Constants
#define RC522_HEADER          0x7F
#define RC522_TAIL_OK         0xF7
#define RC522_TAIL_ERR        0x55

class RC522_UART {
public:
    /**
     * @brief Constructor
     * @param serialStream A pointer to the Stream object (e.g. &Serial1, &mySoftwareSerial)
     */
    RC522_UART(Stream* serialStream);

    /**
     * @brief Call this in setup() to initialize the module mode
     */
    bool begin();

    // ----- Basic Commands -----
    bool reset();
    bool antennaOn();
    bool antennaOff();
    bool halt();

    /**
     * @brief Request for cards in the field
     * @param reqMode RC522_REQ_ALL (0x52) or RC522_REQ_NOT_HALT (0x26)
     * @param tagType Pointer to store the 2-byte tag type returned by the module
     * @return true if successful
     */
    bool request(uint8_t reqMode, uint16_t* tagType = nullptr);

    /**
     * @brief Anti-collision, reads the UID of the card
     * @param uid Array to store the 4-byte UID
     * @return true if successful
     */
    bool anticoll(uint8_t* uid);

    /**
     * @brief Selects a card using its UID
     * @param uid The 4-byte UID array
     * @return true if successful
     */
    bool select(uint8_t* uid);

    /**
     * @brief Authenticate a block using a key
     * @param authMode RC522_AUTH_KEY_A (0x60) or RC522_AUTH_KEY_B (0x61)
     * @param blockAddr Block address (0-63)
     * @param key The 6-byte password key (default usually 0xFF FF FF FF FF FF)
     * @param uid The 4-byte card UID
     * @return true if successful
     */
    bool authState(uint8_t authMode, uint8_t blockAddr, uint8_t* key, uint8_t* uid);

    /**
     * @brief Read data from an authenticated block
     * @param blockAddr Block address to read
     * @param data Array to store the 16 bytes read
     * @return true if successful
     */
    bool readBlock(uint8_t blockAddr, uint8_t* data);

    /**
     * @brief Write data to an authenticated block
     * @param blockAddr Block address to write
     * @param data The 16-byte data array to write
     * @return true if successful
     */
    bool writeBlock(uint8_t blockAddr, uint8_t* data);

    /**
     * @brief Perform an increment/decrement value operation on a block
     * @param mode RC522_VALUE_DEDUCT (0xC0) or RC522_VALUE_RECHARGE (0xC1)
     * @param blockAddr Wallet block address
     * @param value 4-byte value to add/subtract (LSB first)
     * @return true if successful
     */
    bool valueOperation(uint8_t mode, uint8_t blockAddr, uint32_t value);

    /**
     * @brief Backup wallet value to another block
     * @param srcBlock Source block address
     * @param targetBlock Target block address
     * @return true if successful
     */
    bool backupValue(uint8_t srcBlock, uint8_t targetBlock);

private:
    Stream* _serial;
    
    // Internal helper to send packets
    void sendPacket(uint8_t cmd, uint8_t* data = nullptr, uint8_t dataLen = 0);
    
    // Internal helper to receive packets
    // returns true if TAIL_OK (0xF7) is received, false if timeout or TAIL_ERR (0x55)
    bool receivePacket(uint8_t* returnData = nullptr, uint8_t expectedReturnLen = 0, uint32_t timeoutMs = 200);
    
    // Clears the serial RX buffer
    void flushRX();
};

#endif
