
#include "thread_pool.h"

void thread_pool::init(size_t num_threads) {
    num_threads_ = num_threads;
}
void thread_pool::wait_all_done() {
}
void thread_pool::wait_and_destroy() {
}
int thread_pool::current_num_threads_working() {
    return 0;
}
