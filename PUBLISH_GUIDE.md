# Hướng dẫn Đăng ký Thư viện lên Arduino Library Manager

Tài liệu này lưu lại các bước chuẩn xác nhất để xuất bản (publish) một thư viện do bạn viết lên hệ thống quản lý thư viện toàn cầu của Arduino (Arduino Library Manager).

## 1. Chuẩn bị Thư viện trên Github
Trước khi gửi yêu cầu, thư viện của bạn phải đáp ứng các tiêu chuẩn sau:
- Thư mục gốc phải chứa file `library.properties` với đầy đủ thông tin (cực kỳ quan trọng là trường `version`).
- Mã nguồn phải nằm trong thư mục `src/` hoặc nằm trực tiếp ở thư mục gốc.
- Các ví dụ minh họa nên nằm trong thư mục `examples/`.
- Phải có file `README.md` mô tả cách sử dụng.

## 2. Đánh Tag Phiên bản (Release)
Hệ thống tự động của Arduino sẽ quét các Tag trên Github để biết bạn có phiên bản mới hay không.
1. Sau khi code xong, mở file `library.properties` và đảm bảo version khớp với phiên bản muốn phát hành (ví dụ: `version=1.0.0`).
2. Commit toàn bộ code lên Github.
3. Tạo một **Tag** mới trùng khớp với version trên.
   - Bằng lệnh: `git tag 1.0.0` và `git push origin 1.0.0`
   - Hoặc tạo **Release** trên giao diện Github.

## 3. Gửi Yêu cầu Đăng ký (Pull Request)
Arduino không duyệt qua Issue nữa, mà dùng hệ thống tự động qua Pull Request (PR) tại kho: [arduino/library-registry](https://github.com/arduino/library-registry).

### 💡 Lưu ý VÀNG để tránh Xung đột (Merge Conflict)
Kho `library-registry` được hàng ngàn người cập nhật mỗi ngày. **Tuyệt đối không dùng lại kho Fork cũ từ vài tháng / vài năm trước**, vì khi đó file `repositories.txt` của bạn đã quá lỗi thời, thêm link vào sẽ bị báo lỗi "Can't automatically merge".

**Quy trình chuẩn như sau:**
1. Mở trang Github của bạn, kiểm tra xem bạn đã từng Fork kho `library-registry` chưa. Nếu có, hãy vào **Settings** của kho đó -> cuộn xuống dưới cùng -> chọn **Delete this repository**.
2. Truy cập kho gốc của Arduino: [https://github.com/arduino/library-registry](https://github.com/arduino/library-registry)
3. Bấm **Fork** để tạo một bản sao mới tinh về tài khoản của bạn.
4. Mở file `repositories.txt` trong kho bạn vừa Fork.
5. Bấm Edit (hình cây bút), cuộn xuống dòng dưới cùng của file, và thêm link Github thư viện của bạn vào:
   ```text
   https://github.com/TCL47-GITHUB/RC522_UART
   ```
6. Bấm **Commit changes** để lưu lại.
7. Chuyển sang tab **Pull requests**, bấm **New pull request**.
8. Bấm **Create pull request** (chắc chắn nó sẽ hiện màu xanh "Able to merge").
9. Viết tiêu đề (ví dụ: *Add RC522_UART library*) và xác nhận.

## 4. Chờ Hệ thống Duyệt
- Ngay khi bạn tạo Pull Request, một con Bot tự động của Arduino (Arduino Lint) sẽ quét kho thư viện của bạn.
- Nếu bạn làm đúng (file properties chuẩn, có tag version), Bot sẽ đánh dấu Tick Xanh (Pass) và tự động Merge PR của bạn vào hệ thống.
- Chờ khoảng 1-2 tiếng để máy chủ Arduino đồng bộ. Sau đó, bất kỳ ai cũng có thể mở phần mềm Arduino IDE -> Library Manager -> Gõ tên thư viện của bạn để tải về.

## 5. Các Lần Cập nhật Sau Này
Sau khi thư viện đã có trên Arduino Library Manager, những lần sau bạn **KHÔNG CẦN** làm lại các bước Đăng ký nữa.
Bạn chỉ cần:
1. Đổi version trong `library.properties` thành số lớn hơn (vd: `1.0.1`).
2. Code tính năng mới và Commit lên Github.
3. Tạo Tag phiên bản mới (vd: `git tag 1.0.1` -> `git push origin 1.0.1`).
4. Hệ thống Arduino sẽ tự động đánh hơi thấy Tag mới và update thư viện của bạn trên toàn cầu sau 1-2 tiếng!
