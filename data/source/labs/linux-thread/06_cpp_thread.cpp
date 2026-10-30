/*
 * Lab: std::thread, std::mutex, lock_guard, std::atomic
 * Topic: Thread
 * Predict before running: What are plain, guarded and atomic? Which one can be wrong?
 * Key idea: std::lock_guard unlocks automatically when it leaves scope (RAII). std::atomic<int>::operator++ is an atomic operation that needs no mutex.
 * Try next: Delete one join() line and run: how does the program end (std::terminate)?
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
    cout << "CPU cores = " << thread::hardware_concurrency() << '\n';
}
