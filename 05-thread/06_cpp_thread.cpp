/*
 * Bài: std::thread, std::mutex, lock_guard, std::atomic
 * Chủ đề: Thread
 * Dự đoán trước khi chạy: plain, guarded, atomic lần lượt bằng bao nhiêu? Cái nào có thể sai?
 * Ý chính: std::lock_guard tự unlock khi ra khỏi scope (RAII). std::atomic<int>::operator++ là thao tác nguyên tử, không cần mutex.
 * Thử thêm: Xoá một dòng join() rồi chạy: chương trình kết thúc thế nào (std::terminate)?
 */
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

constexpr int N = 1000000;
int plain = 0;
int guarded = 0;
atomic<int> atom{0};
mutex mtx;

template <class F>
void run2(F f)
{
    thread a(f), b(f);
    a.join();
    b.join();
}

int main()
{
    run2([] { for (int i = 0; i < N; i++) plain++; });
    run2([] {
        for (int i = 0; i < N; i++) {
            lock_guard<mutex> lk(mtx);
            guarded++;
        }
    });
    run2([] { for (int i = 0; i < N; i++) atom++; });

    cout << "plain   = " << plain << '\n';
    cout << "guarded = " << guarded << '\n';
    cout << "atomic  = " << atom << '\n';
    cout << "so nhan CPU = " << thread::hardware_concurrency() << '\n';
}
