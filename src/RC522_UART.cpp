#include "RC522_UART.h"

RC522_UART::RC522_UART(Stream* serialStream) {
    _serial = serialStream;
}

bool RC522_UART::begin() {
    // Send 0x01 to enter Normal Mode
    _serial->write(0x01);
    delay(50); // wait a bit for module to process
    
    if (_serial->available()) {
        uint8_t res = _serial->read();
        // Module returns 0xAA on success
        if (res == 0xAA) {
            flushRX();
            return true;
        }
    }
    
    // In case it's already in normal mode, the response might be missing, 
    // we can assume true or do a reset
    return reset();
}

void RC522_UART::flushRX() {
    while (_serial->available()) {
        _serial->read();
    }
}

void RC522_UART::sendPacket(uint8_t cmd, uint8_t* data, uint8_t dataLen) {
    flushRX(); // clear buffer before sending
    
    _serial->write(RC522_HEADER);
    _serial->write(cmd);
    
    for (uint8_t i = 0; i < dataLen; i++) {
        _serial->write(data[i]);
    }
    
    _serial->write(RC522_TAIL_OK);
}

bool RC522_UART::receivePacket(uint8_t* returnData, uint8_t expectedReturnLen, uint32_t timeoutMs) {
    uint32_t startMs = millis();
    uint8_t rxBuffer[32]; // Max expected response is 16 bytes data + 2 bytes headers = 18 bytes
    uint8_t rxIndex = 0;
    
    uint8_t expectedTotalLen = expectedReturnLen + 2; // + Header + Tail
    
    while ((millis() - startMs) < timeoutMs) {
        if (_serial->available()) {
            rxBuffer[rxIndex++] = _serial->read();
            
            // Check if we received enough bytes
            if (rxIndex >= expectedTotalLen) {
                // Verify Header
                if (rxBuffer[0] != RC522_HEADER) return false;
                
                // Verify Tail
                uint8_t tail = rxBuffer[rxIndex - 1];
                if (tail == RC522_TAIL_OK) {
                    if (returnData != nullptr && expectedReturnLen > 0) {
                        for (uint8_t i = 0; i < expectedReturnLen; i++) {
                            returnData[i] = rxBuffer[1 + i]; // Skip header
                        }
                    }
                    return true;
                } else if (tail == RC522_TAIL_ERR) {
                    return false;
                }
                
                // If tail is not OK nor ERR, it's malformed
                return false;
            }
        }
    }
    return false; // Timeout
}

bool RC522_UART::reset() {
    sendPacket(RC522_CMD_RESET);
    return receivePacket();
}

bool RC522_UART::antennaOn() {
    sendPacket(RC522_CMD_ANT_ON);
    return receivePacket();
}

bool RC522_UART::antennaOff() {
    sendPacket(RC522_CMD_ANT_OFF);
    return receivePacket();
}

bool RC522_UART::halt() {
    sendPacket(RC522_CMD_HALT);
    return receivePacket();
}

bool RC522_UART::request(uint8_t reqMode, uint16_t* tagType) {
    sendPacket(RC522_CMD_REQUEST, &reqMode, 1);
    
    uint8_t tagData[2];
    if (receivePacket(tagData, 2)) {
        if (tagType != nullptr) {
            *tagType = (tagData[0] << 8) | tagData[1];
        }
        return true;
    }
    return false;
}

bool RC522_UART::anticoll(uint8_t* uid) {
    sendPacket(RC522_CMD_ANTICOLL);
    return receivePacket(uid, 4);
}

bool RC522_UART::select(uint8_t* uid) {
    sendPacket(RC522_CMD_SELECT, uid, 4);
    return receivePacket();
}

bool RC522_UART::authState(uint8_t authMode, uint8_t blockAddr, uint8_t* key, uint8_t* uid) {
    uint8_t data[12];
    data[0] = authMode;
    data[1] = blockAddr;
    
    // Copy 6-byte key
    for (int i = 0; i < 6; i++) {
        data[2 + i] = key[i];
    }
    
    // Copy 4-byte UID
    for (int i = 0; i < 4; i++) {
        data[8 + i] = uid[i];
    }
    
    sendPacket(RC522_CMD_AUTH, data, 12);
    return receivePacket(nullptr, 0, 500); // Authentication might take longer?
}

bool RC522_UART::readBlock(uint8_t blockAddr, uint8_t* data) {
    sendPacket(RC522_CMD_READ, &blockAddr, 1);
    return receivePacket(data, 16);
}

bool RC522_UART::writeBlock(uint8_t blockAddr, uint8_t* data) {
    uint8_t payload[17];
    payload[0] = blockAddr;
    for (int i = 0; i < 16; i++) {
        payload[1 + i] = data[i];
    }
    
    sendPacket(RC522_CMD_WRITE, payload, 17);
    return receivePacket();
}

bool RC522_UART::valueOperation(uint8_t mode, uint8_t blockAddr, uint32_t value) {
    uint8_t data[6];
    data[0] = mode;
    data[1] = blockAddr;
    // value is LSB first
    data[2] = value & 0xFF;
    data[3] = (value >> 8) & 0xFF;
    data[4] = (value >> 16) & 0xFF;
    data[5] = (value >> 24) & 0xFF;
    
    sendPacket(RC522_CMD_VALUE, data, 6);
    return receivePacket();
}

bool RC522_UART::backupValue(uint8_t srcBlock, uint8_t targetBlock) {
    uint8_t data[2] = {srcBlock, targetBlock};
    sendPacket(RC522_CMD_BAK_VALUE, data, 2);
    return receivePacket();
}
