# ALP ch.4: Thread

## Lý thuyết

Theo *Advanced Linux Programming*, chương 4 "Threads". Lab: `10-alp/09`, `10`, `11`, xem thêm `05-thread/*`. Chủ đề "Thread và đồng bộ hoá" ở trên có phần tổng quan về race condition và deadlock.

### Tạo thread (4.1)
- Thread chạy bên trong một process và **dùng chung** bộ nhớ, fd, môi trường... với các thread khác của process đó; mỗi thread có **stack và thanh ghi riêng**. Header `<pthread.h>`, biên dịch/link với `-pthread` (sách dùng `-lpthread`).
- `pthread_create(&thread_id, attr, thread_function, arg)`: hàm thread có dạng `void *f(void *)`. Hàm pthread trả về **0 hoặc mã lỗi** (không dùng errno).
- **Truyền dữ liệu**: chỉ một con trỏ `void *` → gói nhiều tham số vào struct (listing 4.2, `struct char_print_parms`). Vùng nhớ mà arg trỏ tới phải **còn sống** trong lúc thread dùng: lỗi kinh điển là truyền struct trên stack của main rồi main return trước khi thread chạy xong.
- **`pthread_join(thread, &ret)`**: chờ thread kết thúc và lấy giá trị trả về (`void *`). Giống `wait` cho process. Thread không được join chính nó.
- Thread kết thúc khi return từ hàm thread, khi gọi **`pthread_exit(value)`**, hoặc khi bị huỷ. Một thread gọi `exit()` thì **cả process** kết thúc. Main return cũng làm cả process (mọi thread) kết thúc.
- `pthread_self()` trả về ID của thread hiện tại; so sánh hai ID bằng **`pthread_equal`** (không dùng `==`).
- **Thuộc tính** (`pthread_attr_t`): `pthread_attr_init`, `pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)`, `pthread_attr_destroy`. Thread **joinable** (mặc định) khi kết thúc giữ lại tài nguyên tới khi được join (giống zombie). Thread **detached** tự dọn khi kết thúc và không join được. `pthread_detach(thread)` chuyển một thread sang detached.

### Huỷ thread (4.2)
- `pthread_cancel(thread)` gửi yêu cầu huỷ; thread bị huỷ trả về **`PTHREAD_CANCELED`** cho pthread_join. Thread bị huỷ vẫn phải được join (trừ khi detached).
- Ba trạng thái:
  - **Asynchronously cancelable**: có thể bị huỷ ở bất cứ lệnh nào (`pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, ...)`). Nguy hiểm vì có thể bị huỷ giữa lúc đang cấp phát hay giữ khoá.
  - **Synchronously cancelable** (deferred, **mặc định**): yêu cầu được giữ lại tới khi thread gặp **cancellation point** (nhiều system call chặn như read, sleep, `pthread_join`, `pthread_cond_wait`, hoặc gọi `pthread_testcancel()` một cách tường minh).
  - **Uncancelable**: `pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old)`; yêu cầu huỷ bị treo cho tới khi bật lại. Dùng để bảo vệ **vùng găng** không được dừng giữa chừng (listing 4.6: chuyển tiền giữa hai tài khoản).
- Sách khuyên: tránh huỷ thread nếu có thể; thay vào đó báo cho thread biết để nó tự thoát.

### Dữ liệu riêng của thread (4.3)
- **Thread-specific data**: mỗi thread có bản riêng của một "biến toàn cục". `pthread_key_create(&key, destructor)` tạo key (một lần), `pthread_setspecific(key, value)` và `pthread_getspecific(key)` để đặt/đọc giá trị của thread hiện tại. Thread chưa đặt gì thì nhận NULL. Ví dụ listing 4.7: mỗi thread ghi log vào file riêng `thread<id>.log`.
- **Destructor** của key được gọi tự động khi thread kết thúc (với giá trị khác NULL), kể cả khi thread bị huỷ.
- (C11/GCC có `__thread`/`_Thread_local` đơn giản hơn; xem lab `05-thread/08_thread_local`.)
- **Cleanup handler**: `pthread_cleanup_push(func, arg)` và `pthread_cleanup_pop(execute)` phải đi **thành cặp trong cùng một khối** (chúng là macro). Handler chạy khi thread bị huỷ hoặc gọi pthread_exit trong khoảng giữa push và pop. `pop(1)` gỡ và chạy handler; `pop(0)` chỉ gỡ. Dùng để giải phóng bộ nhớ, mở khoá mutex khi bị huỷ.
- **C++**: `pthread_exit` không đảm bảo gọi destructor của object local. Sách khuyên ném một exception đặc biệt và bắt ở hàm thread ngoài cùng, rồi mới gọi pthread_exit. Nhờ đó stack được unwind đúng cách.

### Đồng bộ và vùng găng (4.4)
- **Race condition**: kết quả phụ thuộc thứ tự chạy của các thread. Ví dụ trong sách: hàng đợi công việc (job queue), hai thread cùng lấy một job hoặc một thread lấy job vừa bị thread khác giải phóng.
- **Mutex** (MUTual EXclusion): `pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;` hoặc `pthread_mutex_init(&m, &attr)`. `pthread_mutex_lock` (chặn nếu đã bị khoá), `pthread_mutex_unlock`. Vùng găng nên càng ngắn càng tốt.
- Ba loại mutex trong sách:
  - **fast** (mặc định): thread lock lại mutex chính nó đang giữ → **deadlock** vĩnh viễn.
  - **recursive**: cho phép lock nhiều lần, đếm số lần; phải unlock bấy nhiêu lần.
  - **error-checking**: lock lại → trả về `EDEADLK`; unlock mutex không sở hữu → `EPERM`.
  - Đặt bằng `pthread_mutexattr_settype` (sách dùng tên GNU `pthread_mutexattr_setkind_np`, `PTHREAD_MUTEX_RECURSIVE_NP`, `PTHREAD_MUTEX_ERRORCHECK_NP`).
- **`pthread_mutex_trylock`**: không chặn; mutex đang bị giữ → trả về `EBUSY` để thread làm việc khác rồi thử lại.
- **Semaphore cho thread** (`<semaphore.h>`): bộ đếm không âm. `sem_init(&s, 0, value)` (tham số 0 = chỉ dùng giữa các thread trong process), `sem_wait` (giảm 1; nếu đang 0 thì chặn), `sem_post` (tăng 1, đánh thức thread đang chờ), `sem_trywait` (không chặn, trả về EAGAIN), `sem_getvalue`, `sem_destroy`. Job queue trong sách: mutex bảo vệ danh sách và semaphore đếm số job, nên thread rảnh chặn chờ thay vì quay vòng.
- **Condition variable**: chờ một điều kiện mà không tốn CPU. `pthread_cond_wait(&cond, &mutex)` **nhả mutex và ngủ trong một thao tác nguyên tử**; khi được đánh thức thì lấy lại mutex trước khi return. `pthread_cond_signal` đánh thức một thread, `pthread_cond_broadcast` đánh thức tất cả. Luôn kiểm tra lại điều kiện trong **vòng while** sau khi thức. Bên đặt điều kiện phải giữ mutex khi đổi biến cờ và signal (listing 4.14, `thread_flag`).
- **Deadlock giữa hai thread**: mỗi thread giữ một khoá và chờ khoá của thread kia. Cách tránh: mọi thread lấy các khoá theo **cùng một thứ tự**. Deadlock cũng có thể xảy ra giữa process và signal handler.

### Cài đặt thread trên Linux (4.5)
- Sách (2001, thư viện **LinuxThreads**): mỗi thread là một **process riêng** tạo bằng system call **`clone`**, dùng chung không gian địa chỉ. Vì vậy mỗi thread có PID khác nhau (`getpid` khác nhau, `ps` thấy nhiều dòng), và có thêm một "manager thread". Signal gửi từ ngoài thường tới process của thread chính; gửi signal tới một thread cụ thể dùng **`pthread_kill(thread, sig)`**.
- Hiện nay (**NPTL**, từ kernel 2.6): mọi thread trong process có **cùng PID** (thread group ID); mỗi thread có TID riêng (`gettid()`, `ps -L`, `/proc/PID/task/`). Không còn manager thread. Signal gửi tới process được giao cho một thread bất kỳ không chặn signal đó. Vẫn dùng `clone` bên dưới.
- `clone` là dạng tổng quát của fork và pthread_create: chọn tài nguyên nào được dùng chung (bộ nhớ, fd, signal handler...). Không nên gọi trực tiếp trong chương trình thông thường.

### Process hay thread? (4.6)
- Thread của một chương trình phải chạy **cùng một file thực thi**; process con có thể exec chương trình khác.
- Thread lỗi (con trỏ hỏng) có thể làm **sập cả process**; process con lỗi không ảnh hưởng process khác → process an toàn hơn.
- Tạo thread nhanh hơn fork (không phải copy bảng trang...). Dữ liệu dùng chung giữa các thread dễ hơn nhưng phải đồng bộ; process thì cần IPC.
- Thread hợp với việc chia nhỏ một tác vụ song song chia sẻ nhiều dữ liệu; process hợp với tác vụ độc lập, cần cô lập lỗi hoặc chạy chương trình khác.

## Trắc nghiệm

### Hàm thread truyền cho pthread_create phải có chữ ký nào?
- [ ] `void f(void)`
- [ ] `int f(int)`
- [x] `void *f(void *)`
- [ ] `void *f(int, char **)`
> Chỉ nhận một `void *` và trả về một `void *`. Muốn nhiều tham số thì gói vào struct.

### Hàm pthread_create báo lỗi như thế nào?
- [ ] Trả về -1 và đặt errno
- [x] Trả về mã lỗi khác 0 (ví dụ EAGAIN); không đặt errno
- [ ] Trả về NULL
- [ ] Ném signal SIGABRT
> Hầu hết các hàm pthread_* đều theo kiểu này. Semaphore (sem_*) thì vẫn dùng -1 và errno.

### Đoạn code có lỗi gì?
```c
void *print(void *arg) { struct parms *p = arg; /* dùng p */ return NULL; }
int main() {
    pthread_t t;
    struct parms p = { 'x', 30000 };
    pthread_create(&t, NULL, print, &p);
    return 0;
}
```
- [ ] Không có lỗi
- [x] main return mà không join: process kết thúc (thread có thể chưa chạy xong) và struct p trên stack của main hết hạn trong khi thread có thể vẫn dùng
- [ ] Không được truyền struct qua void*
- [ ] Phải gọi pthread_exit trong print
> Listing 4.2 trong sách có đúng lỗi này; listing 4.3 sửa bằng cách pthread_join trước khi return.

### Lấy giá trị trả về của thread bằng cách nào?
- [ ] Đọc biến toàn cục `pthread_retval`
- [x] Tham số thứ hai của `pthread_join(thread, &ret)`
- [ ] Giá trị trả về của pthread_create
- [ ] `pthread_self()`
> Thread trả về bằng `return ptr;` hoặc `pthread_exit(ptr)`. Không trả về con trỏ tới biến local của thread.

### Một thread (không phải main) gọi `exit(0)`. Điều gì xảy ra?
- [ ] Chỉ thread đó kết thúc
- [x] Toàn bộ process (mọi thread) kết thúc
- [ ] Lỗi biên dịch
- [ ] Thread trở thành zombie
> Muốn chỉ kết thúc thread hiện tại thì dùng pthread_exit hoặc return từ hàm thread.

### Vì sao so sánh thread ID bằng pthread_equal thay vì `==`?
- [ ] pthread_equal nhanh hơn
- [x] pthread_t là kiểu "opaque", có thể là struct trên một số hệ thống nên không so sánh bằng `==` được
- [ ] `==` luôn trả về false
- [ ] Vì pthread_t là chuỗi
> Trên Linux pthread_t là `unsigned long`, nhưng code portable phải dùng pthread_equal.

### Thread detached khác thread joinable ở điểm nào?
- [ ] Detached chạy nhanh hơn
- [x] Detached tự giải phóng tài nguyên khi kết thúc và không join được; joinable giữ tài nguyên tới khi được join
- [ ] Detached không dùng chung bộ nhớ với process
- [ ] Detached không bị huỷ được
> Tạo bằng thuộc tính `PTHREAD_CREATE_DETACHED` hoặc gọi `pthread_detach`. Thread joinable không ai join thì giống zombie.

### Trạng thái huỷ mặc định của một thread mới tạo là?
- [ ] Asynchronously cancelable
- [x] Synchronously (deferred) cancelable: chỉ bị huỷ tại cancellation point
- [ ] Uncancelable
- [ ] Không thể huỷ
> Đổi bằng `pthread_setcanceltype` (async/deferred) và `pthread_setcancelstate` (enable/disable).

### Hàm nào là cancellation point tường minh, dùng để thread tự kiểm tra yêu cầu huỷ trong vòng lặp tính toán dài?
- [ ] `pthread_yield`
- [x] `pthread_testcancel`
- [ ] `pthread_cancel`
- [ ] `sched_yield`
> Vòng lặp tính toán thuần không gọi system call nào thì không có cancellation point, nên yêu cầu huỷ deferred sẽ không bao giờ có hiệu lực nếu không gọi pthread_testcancel.

### Thread đang trong vùng `PTHREAD_CANCEL_DISABLE` thì nhận pthread_cancel. Điều gì xảy ra?
- [ ] Bị huỷ ngay lập tức
- [ ] Yêu cầu bị bỏ qua vĩnh viễn
- [x] Yêu cầu được giữ lại; khi thread bật lại cancel và gặp cancellation point thì mới bị huỷ
- [ ] pthread_cancel trả về lỗi EBUSY
> Đây là cách bảo vệ một vùng găng phải làm trọn vẹn (listing 4.6: chuyển tiền).

### pthread_join một thread đã bị huỷ thì nhận được giá trị trả về nào?
- [ ] NULL
- [ ] -1
- [x] `PTHREAD_CANCELED`
- [ ] Giá trị cuối cùng thread định return
> PTHREAD_CANCELED là một hằng con trỏ đặc biệt, thường là `(void *)-1`.

### Thread-specific data giải quyết vấn đề gì?
- [ ] Chia sẻ dữ liệu nhanh giữa các thread
- [x] Mỗi thread có bản riêng của cùng một "biến toàn cục" (ví dụ file log riêng), truy cập qua cùng một key
- [ ] Khoá dữ liệu khỏi race condition
- [ ] Truyền dữ liệu từ thread về main
> pthread_key_create một lần, rồi mỗi thread pthread_setspecific giá trị của mình. pthread_getspecific trả về giá trị của thread đang gọi.

### Destructor truyền vào `pthread_key_create` được gọi khi nào?
- [ ] Khi gọi pthread_key_delete
- [x] Khi mỗi thread kết thúc, nếu giá trị của key trong thread đó khác NULL
- [ ] Khi process kết thúc
- [ ] Khi gọi pthread_setspecific lần thứ hai
> Destructor chạy cả khi thread bị huỷ hoặc gọi pthread_exit. Dùng để đóng file, giải phóng bộ nhớ riêng của thread.

### Điều nào ĐÚNG về pthread_cleanup_push/pop? (chọn tất cả đáp án đúng)
- [x] Phải dùng thành cặp trong cùng một khối lệnh
- [x] Handler chạy khi thread bị huỷ trong khoảng giữa push và pop
- [x] `pthread_cleanup_pop(1)` vừa gỡ vừa chạy handler
- [ ] Handler chạy khi thread return bình thường từ giữa khoảng push/pop
> push/pop là macro mở và đóng ngoặc nhọn. Return giữa chừng là undefined behavior; pthread_exit thì có chạy handler.

### Trong C++, vì sao sách khuyên không gọi pthread_exit trực tiếp ở giữa hàm thread?
- [ ] Vì pthread_exit không có trong C++
- [x] Vì có thể không gọi destructor của các object local trên stack của thread; nên ném exception rồi bắt ở hàm thread ngoài cùng
- [ ] Vì pthread_exit làm cả process kết thúc
- [ ] Vì pthread_exit không trả về được giá trị
> Listing 4.9: lớp `ThreadExitException`, ném ra khi muốn thoát; hàm thread bắt exception đó rồi mới gọi pthread_exit. Nhờ đó stack được unwind và các destructor (RAII) chạy.

### Thread lock một mutex mặc định (fast) mà chính nó đang giữ. Điều gì xảy ra?
- [ ] Trả về EDEADLK
- [ ] Lock thành công, bộ đếm tăng lên
- [x] Deadlock: thread chặn mãi mãi
- [ ] Trả về EBUSY
> Error-checking mutex sẽ trả về EDEADLK; recursive mutex sẽ cho lock lại.

### Loại mutex nào cho phép cùng một thread lock nhiều lần?
- [ ] fast
- [x] recursive
- [ ] error-checking
- [ ] Không loại nào
> Mỗi lần lock phải có một lần unlock tương ứng; mutex chỉ thực sự được nhả khi bộ đếm về 0.

### `pthread_mutex_trylock` trả về gì khi mutex đang bị thread khác giữ?
- [ ] Chặn tới khi lấy được
- [x] EBUSY ngay lập tức
- [ ] EAGAIN và đặt errno
- [ ] 0
> Dùng khi thread có việc khác để làm thay vì đứng chờ.

### `sem_init(&s, 0, 3)`: tham số thứ hai bằng 0 và thứ ba bằng 3 nghĩa là?
- [ ] Semaphore dùng giữa các process, giá trị tối đa 3
- [x] Semaphore chỉ dùng giữa các thread trong process, giá trị khởi đầu 3
- [ ] Semaphore nhị phân, có 3 thread chờ
- [ ] Timeout 0 giây, thử lại 3 lần
> pshared khác 0 thì semaphore dùng được giữa các process, khi đó phải nằm trong vùng nhớ chia sẻ.

### Thread gọi `sem_wait` khi giá trị semaphore đang là 0. Điều gì xảy ra?
- [ ] Giá trị thành -1
- [x] Thread chặn cho tới khi thread khác sem_post
- [ ] Trả về lỗi ngay
- [ ] Semaphore bị reset về giá trị khởi đầu
> Giá trị semaphore không bao giờ âm. `sem_trywait` thì không chặn mà trả về -1 với errno = EAGAIN.

### Trong job queue của sách, vì sao dùng thêm semaphore đếm số job ngoài mutex?
- [ ] Để thay thế mutex
- [x] Để thread rảnh chặn chờ khi hàng đợi rỗng thay vì liên tục lock mutex kiểm tra (busy waiting), và thức dậy đúng khi có job mới
- [ ] Để giới hạn số thread
- [ ] Để tăng tốc malloc
> Mutex bảo vệ danh sách; semaphore đếm số phần tử, nên sem_wait thành công thì chắc chắn có job để lấy.

### `pthread_cond_wait(&cond, &mutex)` làm gì?
- [ ] Chờ mutex được nhả
- [x] Nhả mutex và ngủ trong một thao tác nguyên tử; khi được đánh thức thì lấy lại mutex trước khi return
- [ ] Đánh thức một thread đang chờ
- [ ] Khoá mutex rồi chờ, giữ mutex trong lúc ngủ
> Thao tác nguyên tử tránh mất tín hiệu: không có khoảng hở giữa lúc kiểm tra điều kiện và lúc bắt đầu ngủ. Phải đang giữ mutex khi gọi.

### Vì sao pthread_cond_wait phải đặt trong vòng `while (!flag)` thay vì `if`?
- [ ] Vì cú pháp bắt buộc
- [x] Vì có thể thức dậy giả (spurious wakeup) hoặc thread khác đã đổi điều kiện trước khi mình lấy lại mutex
- [ ] Để chờ lâu hơn
- [ ] Vì cond_wait chỉ chờ được một lần
> Sau khi thức luôn kiểm tra lại điều kiện.

### Khác nhau giữa `pthread_cond_signal` và `pthread_cond_broadcast`?
- [ ] signal dùng cho process, broadcast dùng cho thread
- [x] signal đánh thức một thread đang chờ; broadcast đánh thức tất cả
- [ ] broadcast nhả mutex, signal thì không
- [ ] Không khác
> Không có thread nào đang chờ thì cả hai đều không làm gì (tín hiệu không được lưu lại, khác với semaphore).

### Hai thread: thread 1 lock A rồi lock B, thread 2 lock B rồi lock A. Rủi ro gì và cách sửa?
- [ ] Race condition; sửa bằng volatile
- [x] Deadlock; sửa bằng cách mọi thread lấy khoá theo cùng một thứ tự (A rồi B)
- [ ] Starvation; sửa bằng nice
- [ ] Không có rủi ro
> Mỗi thread giữ một khoá và chờ khoá còn lại. Có thể dùng thêm trylock rồi nhả hết để thử lại.

### Theo sách (thư viện LinuxThreads), gọi getpid() trong các thread khác nhau của cùng chương trình thì sao?
- [ ] Luôn giống nhau
- [x] Mỗi thread trả về PID khác nhau vì mỗi thread là một process tạo bằng clone
- [ ] Trả về thread ID
- [ ] Lỗi vì getpid không thread-safe
> Đó là Linux năm 2001. Với NPTL hiện nay, mọi thread có cùng PID (thread group ID) và mỗi thread có TID riêng (`gettid()`, xem bằng `ps -L`).

### System call nào Linux dùng để tạo thread, là dạng tổng quát của fork cho phép chọn tài nguyên dùng chung?
- [ ] `vfork`
- [x] `clone`
- [ ] `spawn`
- [ ] `exec`
> Ví dụ cờ CLONE_VM (chung bộ nhớ), CLONE_FILES (chung bảng fd), CLONE_SIGHAND, CLONE_THREAD. Sách khuyên không gọi clone trực tiếp.

### Gửi signal tới một thread cụ thể trong cùng chương trình bằng hàm nào?
- [ ] `kill`
- [x] `pthread_kill`
- [ ] `raise`
- [ ] `sigqueue`
> `pthread_kill(thread_id, SIGUSR1)`. kill() gửi tới cả process.

### Trường hợp nào nên dùng nhiều process thay vì nhiều thread? (chọn tất cả đáp án đúng)
- [x] Cần chạy một chương trình (file thực thi) khác
- [x] Cần cô lập lỗi: một phần hỏng không được làm sập phần còn lại
- [ ] Cần chia sẻ nhiều cấu trúc dữ liệu lớn liên tục
- [ ] Cần tạo và huỷ đơn vị thực thi thật nhanh, rất nhiều lần
> Chia sẻ dữ liệu nhiều và tạo/huỷ nhanh là lợi thế của thread.
