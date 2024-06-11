
#include "thread_pool.h"
#include <iostream>

void thread_pool::wait_all_done() {
    std::unique_lock<std::mutex> lock(m1_);
    while (current_working_threads_num_ > 0 || !tasks_queue_.empty()) {
        cv_.wait(lock, [&] {
            return tasks_queue_.empty() && current_working_threads_num_ == 0;
        });
        // 1.释放锁
        // 2.等待条件变量被唤醒
        // 3.重新获得锁，继续当前
    }
}
void thread_pool::wait_and_destroy() {
    if (is_shutdown_) { return; }
    wait_all_done();
    std::unique_lock<std::mutex> lock(m1_);
    is_shutdown_ = true;
    cv_.notify_all();
}
size_t thread_pool::current_num_threads_working() const {
    std::unique_lock<std::mutex> lock(m1_);
    return current_working_threads_num_;
}
void thread_pool::init() {
    for (int i = 0; i < num_threads_; ++i) {
        // worker_thread 重载为可调用对象，可以理解为传入了无参的
        // worker_thread()
        threads_[i] = std::thread(worker_thread(i, this));
    }
    current_working_threads_num_ = 0;
}
