# web — giao diện ôn bài

```sh
make web            # hoặc: npm start  /  python3 web/backend/server.py [--port 8000]
```
Mở http://localhost:8000. Không cần cài gì thêm: backend chỉ dùng thư viện chuẩn của Python 3, frontend là HTML/CSS/JS thuần (không build).

## Cấu trúc

```
web/
├── backend/                 Python 3, thư viện chuẩn
│   ├── server.py            HTTP: phục vụ frontend + JSON API, tự khởi động lại khi file .py đổi
│   ├── config.py            đường dẫn, thư mục bài, giới hạn thời gian/output
│   ├── labs.py              đọc bài lab (comment đầu file nguồn) + ANSWERS.md
│   ├── theory.py            đọc 09-theory/*.md → lý thuyết + câu trắc nghiệm
│   ├── runner.py            build bằng Makefile gốc, chạy bài trong pty, giữ output
│   └── jobwrap.py           chạy chương trình như shell chạy foreground job (Ctrl+C/Z, orphan)
└── frontend/
    ├── index.html           khung trang
    ├── css/style.css        giao diện (sáng/tối)
    └── js/                  nạp theo thứ tự trong index.html, dùng chung biến toàn cục
        ├── core.js          h(), api(), lưu tiến độ (localStorage), trạng thái chung
        ├── render.js        tô màu code, markdown
        ├── terminal.js      terminal trong trình duyệt + Terminal 2
        ├── nav.js           thanh bên, màu chủ đề, icon
        ├── home.js          trang Tổng quan
        ├── lab.js           trang bài lab + diff dự đoán/output
        ├── theory.js        trang lý thuyết
        ├── quiz.js          trắc nghiệm
        └── main.js          khởi động + định tuyến (#home, #lab/<id>, #theory/<id>, #quiz)
```

Nội dung không nằm trong `web/`: bài lab ở `01-c` … `08-shell`, đáp án ở `ANSWERS.md`, lý thuyết + trắc nghiệm ở `09-theory/`. Backend đọc lại các file này mỗi lần gọi API, nên sửa nội dung chỉ cần tải lại trang.

## API

| Method | Đường dẫn | Mô tả |
| --- | --- | --- |
| GET | `/api/labs` | danh sách bài, mã nguồn, câu hỏi, đáp án |
| GET | `/api/theory` | các chủ đề lý thuyết và câu trắc nghiệm |
| POST | `/api/runs` | `{id}`: build & chạy bài; `{terminal: true}`: mở bash |
| GET | `/api/runs/<run>?from=N` | output từ chunk N, trạng thái, PID |
| POST | `/api/runs/<run>/input` | `{text}` gửi vào stdin |
| POST | `/api/runs/<run>/key` | `{key: "c" \| "z" \| "\\" \| "d"}` như Ctrl+C/Z/\/D |
| POST | `/api/runs/<run>/job` | `{cmd: "fg" \| "bg"}` |
| POST | `/api/runs/<run>/signal` | `{sig: "TERM" \| "USR1" \| …}` gửi tới PID của bài |
| POST | `/api/runs/<run>/ps` | in `ps` của các tiến trình trong lần chạy |
| POST | `/api/runs/<run>/stop` | kill -9 toàn bộ |

Server chỉ lắng nghe `127.0.0.1` và từ chối request có Host/Origin lạ (vì có endpoint mở bash). POST phải là `application/json`.
