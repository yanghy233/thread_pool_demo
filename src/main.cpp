/*
 * WHAT THIS EXAMPLE DOES]
 *
 * As soon as we add the tasks to the pool, the threads will run them. It can
 * happen that you see a single thread running all the tasks (highly unlikely).
 * It is up the OS to decide which thread will run what. So it is not an error
 * of the thread pool but rather a decision of the OS.
 *
 * */

#include <iostream>
#include "thread_pool/thread_pool.h"

void task(void *arg) {
    printf("Thread working on %d\n", (int)(intptr_t)arg);
}

void task2(int i) {
    printf("i = %d\n", i);
}

int main() {
    size_t const thread_count = std::thread::hardware_concurrency();
    std::cout << "Initialize with " << thread_count << " threads" << std::endl;
    thread_pool threadPool(thread_count);

    std::cout << "Adding 40 tasks to thread pool" << std::endl;
    int i;
    for (i = 0; i < 40; i++) {
        auto future = threadPool.submit_task(task, (void *)(uintptr_t)i);
        //        auto future = threadPool.submit_task(task2, i);
    };

    std::cout << "current number of working threads: "
              << threadPool.current_num_threads_working() << std::endl;

    threadPool.wait_all_done();

    std::cout << "destroy thread pool..." << std::endl;
    threadPool.wait_and_destroy();
    std::cout << "Done." << std::endl;

    return 0;
}