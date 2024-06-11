/*
 * WHAT THIS EXAMPLE DOES
 *
 * We create a pool of 4 threads and then add 40 tasks to the pool(20 task1
 * functions and 20 task2 functions). task1 and task2 simply print which thread
 * is running them.
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

int main() {
    std::cout << "Initialize with 8 threads" << std::endl;
    thread_pool threadPool;
    threadPool.init(8);

    std::cout << "Adding 40 tasks to thread pool" << std::endl;
    int i;
    for (i = 0; i < 40; i++) {
        auto future = threadPool.submit_task(task, (void *)(uintptr_t)i);
        if (future.get() == 0) {
            std::cerr << "submit task failed." << std::endl;
            return -1;
        }
    };

    std::cout << "current working threads number: "
              << threadPool.current_num_threads_working() << std::endl;

    threadPool.wait_all_done();

    std::cout << "destroy thread pool..." << std::endl;
    threadPool.wait_and_destroy();
    std::cout << "Done" << std::endl;

    return 0;
}