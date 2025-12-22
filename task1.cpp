#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>
#include "DynamicArray.h" 

using namespace std;

DynamicArray<char> shared_buffer;
const int ITERATIONS = 500000;
const int NUM_THREADS = 10;

// Mutex
mutex mtx;
void task_mutex(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        lock_guard<mutex> lock(mtx);
        shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
    }
}

// Semaphore
counting_semaphore<1> sem(1);
void task_semaphore(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        sem.acquire();
        shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
        sem.release();
    }
}

// SpinLock
atomic_flag spin_lock_flag = ATOMIC_FLAG_INIT;
void task_spinlock(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        while (spin_lock_flag.test_and_set(memory_order_acquire)) { 
        }
        shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
        spin_lock_flag.clear(memory_order_release);
    }
}

// SpinWait
atomic_flag spin_wait_flag = ATOMIC_FLAG_INIT;
void task_spinwait(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        while (spin_wait_flag.test_and_set(memory_order_acquire)) {
            this_thread::yield(); 
        }
        shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
        spin_wait_flag.clear(memory_order_release);
    }
}

// Monitor
class Monitor {
private:
    mutex mon_mtx;
public:
    void safe_push() {
        lock_guard<mutex> lock(mon_mtx); 
        shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
    }
} monitor_instance;

void task_monitor(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        monitor_instance.safe_push();
    }
}

// Barrier
barrier sync_point(NUM_THREADS);
mutex barrier_mtx; 

void task_barrier(int id) {
    for (int i = 0; i < ITERATIONS; ++i) {
        sync_point.arrive_and_wait(); 
        {
            lock_guard<mutex> lock(barrier_mtx);
            shared_buffer.push_back(static_cast<char>('A' + (rand() % 26)));
        }
        sync_point.arrive_and_wait();
    }
}
template<typename Func>
void benchmark(const string& name, Func func) {
    shared_buffer.clear();
    auto start = chrono::high_resolution_clock::now();
    
    DynamicArray<thread> threads; 
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(func, i);
    }
    
    for (int i = 0; i < threads.get_size(); ++i) {
        if(threads[i].joinable()) threads[i].join();
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end - start;
    
    cout << left << setw(10) << name << ": " << fixed << setprecision(3) << elapsed.count() << " ms. " << "\n";
}

int main() {
    cout << "ЗАДАНИЕ 1: Сравнительный анализ примитивов" << endl;
    cout << "Кол-во потоков: " << NUM_THREADS << ", Итераций: " << ITERATIONS << endl << endl;

    benchmark("Mutex", task_mutex);
    benchmark("Semaphore", task_semaphore);
    benchmark("Monitor", task_monitor);
    benchmark("SpinLock", task_spinlock);
    benchmark("SpinWait", task_spinwait);
    benchmark("Barrier", task_barrier); 

    return 0;
}
