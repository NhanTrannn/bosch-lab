# Thread và đồng bộ hoá

## Lý thuyết

### Thread vs process
| | Process | Thread |
| --- | --- | --- |
| Không gian địa chỉ | riêng | dùng chung trong process |
| Tạo/huỷ, context switch | đắt hơn | rẻ hơn |
| Giao tiếp | cần IPC | đọc/ghi biến chung (cần đồng bộ) |
| Lỗi (segfault) | chỉ chết process đó | chết **cả process** |

- Thread chung: code, data/bss, heap, fd, signal handler, PID. Riêng: stack, thanh ghi/PC, TID, signal mask, errno, biến `thread_local`.
- POSIX: `pthread_create`, `pthread_join` (chờ + lấy giá trị trả về), `pthread_detach` (tự dọn khi kết thúc, không join được nữa), `pthread_exit`, `pthread_self`. Biên dịch với `-pthread`.
- C++11: `std::thread` (phải join hoặc detach trước khi huỷ, không thì `std::terminate`), `std::mutex`, `std::lock_guard`, `std::unique_lock`, `std::scoped_lock` (C++17, khoá nhiều mutex không deadlock), `std::condition_variable`, `std::atomic`, `std::async/future`.

### Race condition và vùng găng
- **Race condition**: kết quả phụ thuộc thứ tự xen kẽ của các thread. `counter++` không atomic: đọc → cộng → ghi.
- **Critical section** cần: mutual exclusion, progress, bounded waiting.
- **Atomic**: thao tác không thể bị chia cắt (`std::atomic<int>`, `__atomic_*`, `stdatomic.h`). Nhanh với biến đơn, không thay được mutex cho thao tác nhiều bước.

### Công cụ đồng bộ
- **Mutex**: khoá có chủ sở hữu, chỉ thread đã lock mới được unlock. Thread chờ bị ngủ (sleep).
- **Spinlock**: chờ bằng vòng lặp bận, không ngủ → hợp với vùng găng rất ngắn, trên đa nhân, trong kernel/ngắt (không được ngủ). Trên đơn nhân thì vô ích.
- **Semaphore**: bộ đếm; `wait/P/sem_wait` giảm (chặn nếu 0), `post/V/sem_post` tăng. Binary semaphore (0/1) giống mutex nhưng **không có chủ sở hữu** (thread khác post được) → hợp cho báo hiệu (signaling) giữa thread/ISR. Counting semaphore giới hạn N tài nguyên.
- **Condition variable**: chờ một điều kiện, luôn đi kèm mutex; `wait` nhả mutex và ngủ một cách atomic, thức dậy thì lấy lại mutex. Luôn kiểm tra điều kiện trong **vòng while** (spurious wakeup, nhiều thread cùng thức). `signal/notify_one` đánh thức một, `broadcast/notify_all` đánh thức tất cả.
- **Read-write lock**: nhiều reader cùng lúc hoặc một writer.
- **Recursive mutex**: cùng thread lock nhiều lần được.
- **Barrier**: đợi đủ N thread mới cùng đi tiếp.

### Deadlock
Bốn điều kiện Coffman (cần **đủ cả bốn**):
1. Mutual exclusion — tài nguyên không chia sẻ được.
2. Hold and wait — giữ một tài nguyên trong khi chờ cái khác.
3. No preemption — không giật được tài nguyên.
4. Circular wait — chờ vòng tròn.

Phòng tránh: khoá theo **thứ tự cố định** (phá circular wait), `trylock` rồi nhả nếu thất bại, khoá tất cả cùng lúc (`std::lock`, `scoped_lock`), timeout. Tránh (avoidance): thuật toán Banker. **Livelock**: các thread vẫn chạy nhưng liên tục nhường nhau, không tiến triển. **Starvation**: một thread không bao giờ được tài nguyên.

### Khái niệm khác
- **Thread-safe**: gọi đồng thời từ nhiều thread vẫn đúng. **Reentrant**: có thể gọi lại khi đang chạy dở (ví dụ từ signal handler/ISR) — không dùng biến static/global, không gọi hàm không reentrant. `strtok` không reentrant → `strtok_r`.
- Producer–consumer (bounded buffer): mutex + 2 condition variable, hoặc 2 semaphore (empty, full) + mutex.
- Công cụ: `valgrind --tool=helgrind`, `-fsanitize=thread`.

## Trắc nghiệm

### Thứ nào các thread trong cùng process KHÔNG dùng chung?
- [ ] Heap
- [ ] Biến toàn cục
- [x] Stack
- [ ] File descriptor
> Mỗi thread có stack, thanh ghi và PC riêng.

### Vì sao `counter++` từ hai thread không có khoá cho kết quả sai?
- [ ] Vì compiler bỏ qua lệnh
- [x] Vì đó là ba bước đọc–cộng–ghi, các thread có thể xen kẽ làm mất lần cộng
- [ ] Vì biến không phải volatile
- [ ] Vì mỗi thread có bản copy riêng của counter
> volatile cũng không sửa được. Dùng mutex hoặc std::atomic.

### Bốn điều kiện cần để xảy ra deadlock là? (chọn tất cả đáp án đúng)
- [x] Mutual exclusion
- [x] Hold and wait
- [x] No preemption
- [x] Circular wait
- [ ] Priority inversion
> Phá một trong bốn là đủ ngăn deadlock. Cách phổ biến nhất: luôn khoá theo cùng thứ tự.

### Cách đơn giản và phổ biến nhất để tránh deadlock khi cần khoá hai mutex A và B?
- [ ] Dùng volatile
- [x] Mọi thread luôn khoá theo cùng một thứ tự (A rồi B)
- [ ] Tăng ưu tiên thread
- [ ] Dùng sleep giữa hai lần khoá
> Hoặc dùng std::lock / std::scoped_lock để khoá cả hai cùng lúc.

### Khác biệt chính giữa mutex và binary semaphore?
- [ ] Không có khác biệt
- [x] Mutex có chủ sở hữu (thread nào lock thì thread đó unlock); semaphore không có, thread khác có thể post
- [ ] Semaphore chỉ dùng được giữa các process
- [ ] Mutex có thể đếm lớn hơn 1
> Vì vậy semaphore hợp với báo hiệu sự kiện (ví dụ ISR báo cho task), mutex hợp với bảo vệ vùng găng. Một số mutex hỗ trợ priority inheritance.

### Counting semaphore khởi tạo bằng 3. Bốn thread lần lượt gọi sem_wait (chưa ai post). Kết quả?
- [ ] Cả bốn đi qua
- [x] Ba thread đi qua, thread thứ tư bị chặn
- [ ] Chỉ một thread đi qua
- [ ] Lỗi
> Mỗi sem_wait giảm 1; khi giá trị là 0 thì sem_wait chặn tới khi có sem_post.

### Vì sao phải gọi `pthread_cond_wait` trong vòng `while (!dieu_kien)` thay vì `if`?
- [ ] Để chạy nhanh hơn
- [x] Vì có thể bị đánh thức giả (spurious wakeup) hoặc thread khác đã làm điều kiện sai trước khi thread này chạy lại
- [ ] Vì cond_wait không nhả mutex
- [ ] Chuẩn POSIX cấm dùng if
> Sau khi thức dậy phải kiểm tra lại điều kiện.

### `pthread_cond_wait(&cond, &mutex)` làm gì với mutex?
- [ ] Không đụng tới mutex
- [x] Nhả mutex và ngủ một cách atomic; khi thức dậy thì lấy lại mutex trước khi trả về
- [ ] Huỷ mutex
- [ ] Khoá mutex lần nữa
> Phải đang giữ mutex trước khi gọi.

### Spinlock phù hợp trong trường hợp nào?
- [ ] Vùng găng dài, có I/O
- [x] Vùng găng rất ngắn trên hệ đa nhân, hoặc ngữ cảnh không được ngủ (xử lý ngắt trong kernel)
- [ ] Hệ đơn nhân không preemption
- [ ] Đồng bộ giữa hai process qua mạng
> Spinlock chờ bận tốn CPU; nếu vùng găng dài thì mutex (ngủ khi chờ) tốt hơn.

### `std::thread t(f);` rồi để t bị huỷ mà chưa join hay detach thì sao?
- [ ] Thread tự detach
- [ ] Thread tự join
- [x] Gọi std::terminate, chương trình bị huỷ
- [ ] Không sao cả
> C++20 có std::jthread tự join trong destructor.

### `std::lock_guard<std::mutex> lk(m);` mang lại lợi ích gì?
- [ ] Tự khoá lại sau mỗi lệnh
- [x] RAII: mutex tự được unlock khi lk ra khỏi scope, kể cả khi có exception
- [ ] Cho phép nhiều thread cùng vào
- [ ] Nhanh hơn atomic
> unique_lock linh hoạt hơn (unlock sớm, dùng với condition_variable), scoped_lock khoá được nhiều mutex.

### Hàm reentrant là hàm:
- [ ] Chỉ được gọi bởi một thread
- [x] Có thể bị ngắt giữa chừng và gọi lại (ví dụ từ ISR/signal handler) mà vẫn đúng
- [ ] Gọi đệ quy chính nó
- [ ] Luôn dùng mutex bên trong
> Không dùng dữ liệu static/global không được bảo vệ, không giữ khoá. Hàm dùng mutex có thể thread-safe nhưng không reentrant (gọi lại từ signal handler sẽ deadlock).

### Livelock khác deadlock ở chỗ:
- [ ] Livelock chỉ xảy ra trên đơn nhân
- [x] Các thread vẫn hoạt động (đổi trạng thái liên tục) nhưng không tiến triển được
- [ ] Livelock tự hết sau một thời gian chắc chắn
- [ ] Không khác gì
> Ví dụ hai thread cùng trylock thất bại, cùng nhả rồi cùng thử lại mãi.

### Bài producer–consumer với bộ đệm N ô dùng semaphore cần những gì?
- [ ] Một mutex là đủ
- [x] Semaphore empty = N, semaphore full = 0, và một mutex bảo vệ bộ đệm
- [ ] Semaphore empty = 0, full = N
- [ ] Hai mutex
> Producer: wait(empty) → lock → đặt → unlock → post(full). Consumer: wait(full) → lock → lấy → unlock → post(empty).

### `std::atomic<int> x; x++;` từ nhiều thread thì:
- [ ] Vẫn bị race condition
- [x] Mỗi lần tăng là atomic, kết quả cuối đúng
- [ ] Không biên dịch được
- [ ] Cần thêm volatile
> Nhưng chuỗi thao tác `if (x == 0) x = 1;` vẫn không atomic → dùng compare_exchange hoặc mutex.

### Biên dịch chương trình pthread bằng gcc cần thêm tuỳ chọn nào?
- [ ] -lthread
- [x] -pthread
- [ ] -fthread
- [ ] -D_THREAD
> -pthread vừa định nghĩa macro cần thiết vừa link thư viện pthread (glibc 2.34+ đã gộp vào libc nhưng vẫn nên dùng).
