# Thư viện Arduino RC522_UART

[🇺🇸 Read in English](README.md)

Thư viện Arduino dành cho module RFID 13.56MHz RC522 Mini giao tiếp UART.

Thư viện này đơn giản hóa việc giao tiếp với module RC522 thông qua giao thức UART được tài liệu hóa cho phiên bản mini. Thư viện hoạt động trên mọi đối tượng `Stream`, nghĩa là bạn có thể dùng cổng Serial phần cứng (`Serial1`, `Serial2`) hoặc `SoftwareSerial`.

## Các tính năng
- **Đọc UID**: Dễ dàng đọc mã thẻ 4-byte của các thẻ Mifare Classic (S50/S70).
- **Xác thực**: Hỗ trợ xác thực mật khẩu Key A và Key B.
- **Đọc & Ghi dữ liệu**: Đọc và ghi các khối (block) 16-byte trên thẻ Mifare Classic.
- **Tính năng Ví điện tử**: Hỗ trợ các lệnh cộng/trừ tiền (PcdValue) và lệnh sao lưu (backup) trên các khối giá trị (Value block) của thẻ Mifare.

## Sơ đồ nối dây (Ví dụ với ESP32)
| Chân Module | Chân ESP32 | Ghi chú |
|------------|-----------|------|
| VCC        | 5V        | Bắt buộc dùng điện áp 5V DC |
| GND        | GND       | |
| TX         | RX1       | Nhận dữ liệu UART |
| RX         | TX1       | Truyền dữ liệu UART |

*Lưu ý: Module sử dụng tốc độ truyền (baudrate) cố định là **115200 bps**.*

## Khởi đầu nhanh
Dưới đây là một ví dụ tối giản để đọc UID của thẻ:

```cpp
#include <Arduino.h>
#include "RC522_UART.h"

// Khởi tạo với cổng Serial1 (vd: ESP32, Mega)
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

## Tài liệu API (Danh sách hàm)

### Khởi tạo & Cấu hình
- `bool begin()`: Khởi tạo module, gửi lệnh để vào chế độ bình thường. Trả về `true` nếu module phản hồi chính xác.
- `bool reset()`: Khởi động lại (soft-reset) chip RC522.
- `bool antennaOn()` / `bool antennaOff()`: Bật hoặc tắt phát sóng ăng-ten.

### Thao tác Thẻ cơ bản
- `bool request(uint8_t reqMode, uint16_t* tagType = nullptr)`: Dò tìm thẻ trong vùng phủ sóng. `reqMode` thường là `RC522_REQ_ALL`. Trả về `true` nếu tìm thấy thẻ.
- `bool anticoll(uint8_t* uid)`: Đọc mã thẻ (UID) 4-byte của thẻ vừa tìm thấy. Trả về `true` nếu thành công.
- `bool select(uint8_t* uid)`: Chọn thẻ có mã UID tương ứng để thực hiện các thao tác chuyên sâu. Trả về `true` nếu thành công.
- `bool halt()`: Đưa thẻ đang được chọn vào chế độ ngủ (sleep) để không bị đọc lặp lại liên tục.

### Bộ nhớ & Bảo mật (Mifare Classic 1K/4K)
- `bool authState(uint8_t authMode, uint8_t blockAddr, uint8_t* key, uint8_t* uid)`: Đăng nhập vào một block sử dụng mật khẩu 6-byte (`RC522_AUTH_KEY_A` hoặc `RC522_AUTH_KEY_B`). BẮT BUỘC phải gọi hàm này trước khi đọc/ghi bất kỳ block nào.
- `bool readBlock(uint8_t blockAddr, uint8_t* recvData)`: Đọc 16 byte từ block được chỉ định và lưu vào mảng `recvData`.
- `bool writeBlock(uint8_t blockAddr, uint8_t* writeData)`: Ghi 16 byte dữ liệu vào block được chỉ định.

### Ví điện tử / Tính toán Giá trị
- `bool valueOperation(uint8_t op, uint8_t blockAddr, uint32_t value)`: Thực hiện phép tính cộng/trừ tiền (`RC522_VALUE_RECHARGE` hoặc `RC522_VALUE_DEDUCT`) trên một block đã được định dạng chuẩn Ví điện tử.
- `bool backupValue(uint8_t sourceBlock, uint8_t destBlock)`: Sao lưu giá trị từ block nguồn sang block đích.

## Các Ví dụ đính kèm (Examples)
1. `ReadUID.ino`: Đoạn code đơn giản để phát hiện thẻ và in mã UID ra màn hình.
2. `ReadWriteBlock.ino`: Thể hiện quy trình chuẩn mực để dò tìm, đăng nhập, đọc và ghi một block dữ liệu.
3. `WalletOperation.ino`: Hướng dẫn cách dùng lệnh PcdValue để nạp tiền, trừ tiền và sao lưu một block "ví điện tử" trên thẻ Mifare Classic.
4. `FormatValueBlock.ino`: Hướng dẫn cách định dạng một block trống trở thành chuẩn Ví điện tử (16 byte) để dùng cho `WalletOperation`.
5. `SoftwareSerial.ino`: Hướng dẫn cách cắm nối module với Arduino Uno sử dụng thư viện `SoftwareSerial` (chân 2 và 3).

## Giấy phép
Giấy phép MIT. Bạn được phép thoải mái sử dụng thư viện này trong các dự án cá nhân và thương mại.
