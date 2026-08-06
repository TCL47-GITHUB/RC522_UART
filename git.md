# Hướng dẫn sử dụng Git cơ bản cho thư viện Arduino

Mỗi khi bạn sửa code, thêm tính năng mới hoặc sửa lỗi, bạn cần dùng các lệnh Git sau để đẩy (push) code mới lên Github.

**⚠️ LƯU Ý QUAN TRỌNG:** Trước khi gõ bất kỳ lệnh `git` nào, bạn phải đảm bảo Terminal/Powershell đang nằm TRONG thư mục chứa code (thư mục `RC522_UART`).
Nếu đang ở ngoài, hãy gõ lệnh này trước:
```bash
cd RC522_UART
```

## 1. Lưu code thông thường (Không đổi phiên bản)
Dùng khi bạn chỉ nháp code, hoặc sửa các lỗi nhỏ lặt vặt (chưa muốn thông báo cho hệ thống Arduino cập nhật).

```bash
# 1. Thêm tất cả các file vừa sửa vào danh sách chuẩn bị
git add .

# 2. Đóng gói (Commit) các thay đổi kèm theo lời nhắn
git commit -m "Ghi chú nội dung bạn vừa sửa vào đây"

# 3. Đẩy code lên nhánh chính (main) của Github
git push origin main
```

## 2. Phát hành phiên bản mới (Cập nhật lên Arduino IDE)
Khi bạn đã hoàn thiện một tính năng lớn và muốn tất cả người dùng Arduino trên toàn thế giới nhận được bản cập nhật này, bạn làm theo các bước sau:

**Bước 1:** Mở file `library.properties` và tăng số version lên (Ví dụ từ `1.0.3` thành `1.0.4`).

**Bước 2:** Lưu code lên Github (giống mục 1)
```bash
git add .
git commit -m "Phát hành phiên bản 1.0.4 - Thêm tính năng XYZ"
git push origin main
```

**Bước 3:** Đánh dấu Tag phiên bản và đẩy Tag lên Github
*Hệ thống của Arduino sẽ tự động quét cái Tag này để cập nhật thư viện của bạn.*
```bash
# Đánh tag cho phiên bản mới (phải khớp với version trong library.properties)
git tag 1.0.4

# Đẩy tag đó lên Github
git push origin 1.0.4
```
*(Chờ khoảng 1 tiếng sau, thư viện của bạn trên Arduino IDE Library Manager sẽ tự động được làm mới)*

## 3. Các lệnh phụ trợ thường dùng

- Xem trạng thái các file bị thay đổi (chưa được commit):
```bash
git status
```

- Xem lịch sử các lần commit:
```bash
git log --oneline
```

- Rút lại lệnh commit vừa làm (nếu lỡ commit nhầm nhưng chưa push):
```bash
git reset HEAD~1
```
