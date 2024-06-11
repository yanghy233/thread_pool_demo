#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <thread>
#include <memory>
#include <future>
#include <queue>
#include <vector>
#include <functional>

class thread_pool {
  private:
    // num of threads
    size_t num_threads_;
    // tasks queue
    std::queue<std::packaged_task<void()>> tasks_;
    // threads vector
    std::vector<std::thread> threads_;

  public:
    thread_pool() = default;

    ~thread_pool() {
        // stop all threads
        for (auto &thread : threads_) { thread.join(); }
        threads_.clear();
    }

    // API 1: initialize
    void init(size_t num_threads);

    // API 2: submit task
    // C++模板类、模板函数不允许放在源文件中实现，只能在头文件写逻辑
    template<typename Func, typename... Args>
    auto submit_task(Func func, Args... args) -> std::future<int> {
        return std::future<int>();
    }

    // API 3: wait all tasks done
    void wait_all_done();

    // API 4: wait and shutdown the thread pool
    void wait_and_destroy();

    // API 5: get the number of threads
    int current_num_threads_working();
};

#endif