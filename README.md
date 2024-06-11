# Design a Thread Pool in C++

用法参考 src/main.cpp

编译运行
```shell
mkdir build && cd build 
cmake .. && make -j$(nproc)
```

API
```c++
// 创建线程池
thread_pool(int thread_count);

// 提交任务
auto submit_task(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>;

// 等待所有任务完成
void wait_all_done();

// 等待并销毁线程池
void wait_and_destroy();

// 当前正在执行任务的线程数量
size_t current_num_threads_working() const;
```

CMakeLists.txt
- src, include 只存储自定义的库文件，只有 main.cpp, example/* 可执行文件

Reference:
https://github.com/Pithikos/C-Thread-Pool