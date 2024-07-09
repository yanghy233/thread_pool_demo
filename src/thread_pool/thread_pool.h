#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <thread>
#include <memory>
#include <future>
#include <queue>
#include <vector>
#include <functional>
#include <iostream>

class thread_pool {
  private:
    // num of threads
    size_t num_threads_;
    // tasks queue
    std::queue<std::function<void()>> tasks_queue_;
    // threads vector
    std::vector<std::thread> threads_;
    // global mutex
    mutable std::mutex m1_;
    // condition variable
    std::condition_variable cv_;
    // is shutdown or not
    bool is_shutdown_;
    // current working threads number
    size_t current_working_threads_num_;

    // 每个 thread 封装一个 worker, worker 负责从 tasks_ 中取出任务并执行
    class worker_thread {
      private:
        int thread_id_;
        thread_pool *pool_;

      public:
        worker_thread(int thread_id, thread_pool *pool)
            : thread_id_(thread_id), pool_(pool) {
                // init: all bitmap set to zero
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);

                // set location of bitmap, i = 1 设置要绑定的 CPU 核心编号
                CPU_SET(thread_id, &cpuset);

                // set thread affinity bind to cpu
                pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            }

        void operator()() {
            // 等待队列中任务不为空，被唤醒后，取任务，做任务
            while (!pool_->is_shutdown_) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(pool_->m1_);
                    while (pool_->tasks_queue_.empty()) {
                        pool_->cv_.wait(lock, [&] {
                            return !pool_->tasks_queue_.empty()
                                || pool_->is_shutdown_;
                        });
                        if (pool_->is_shutdown_) {
                            return;
                        }
                    }
                    task = pool_->tasks_queue_.front();
                    pool_->tasks_queue_.pop();
                }

                pool_->m1_.lock();
                pool_->current_working_threads_num_++;
                pool_->m1_.unlock();
                // execute
                task();
                pool_->m1_.lock();
                pool_->current_working_threads_num_--;
                if (pool_->current_working_threads_num_ == 0) {
                    // wake up wait_all_done()  由于只有一个 mutex,但有两处 wait
                    pool_->cv_.notify_all();
                }
                pool_->m1_.unlock();
            }
        }
    };

  public:
    // API 1: initialize
    explicit thread_pool(size_t num_threads)
        : num_threads_(num_threads), threads_(num_threads) {
        init();
        is_shutdown_ = false;
    }

    ~thread_pool() {
        is_shutdown_ = true;
        cv_.notify_all();
        // stop all threads
        for (auto &thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads_.clear();
    }

    void init();

    // API 2: submit task
    // C++模板类、模板函数不允许放在源文件中实现，只能在头文件写逻辑
    template<typename Func, typename... Args>
    auto submit_task(Func &&func, Args &&...args)
        -> std::future<decltype(func(args...))> {
        // 1.构造 std::function
        // 模板函数，只能用 decltype
        // 推导获取返回值类型,并且将传入的参数与函数直接绑定
        std::function<decltype(func(args...))()> f1 =
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

        // 2.create a packaged_task
        std::packaged_task<decltype(func(args...))()> task(std::move(f1));

        // 3.create the share_ptr of task, enable to execute in other thread
        auto task_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()>>(std::move(task));

        // 4.再次封装成 void()
        std::function<void()> function = [task_ptr]() {
            (*task_ptr)();
        };

        // 5.get the future 最后的计算结果
        auto future = task_ptr->get_future();

        // 6.send it to the queue
        {
            std::lock_guard<std::mutex> lock(m1_);
            tasks_queue_.push(function);
        }
        cv_.notify_one();

        return future;
    }

    // API 3: wait all tasks done
    void wait_all_done();

    // API 4: wait and shutdown the thread pool
    void wait_and_destroy();

    // API 5: get the number of threads
    size_t current_num_threads_working() const;
};

#endif