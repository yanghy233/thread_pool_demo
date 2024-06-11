## Design a thread pool in C++ 

CMakeLists.txt
- src, include 只存储自定义的库文件，只有 main.cpp, example/* 可执行

编译运行
```shell
mkdir build && cd build 
cmake .. && make -j$(nproc)
```

API:
```c++
thread_pool(int thread_count);
auto submit_task(Func &&func, Args &&...args) -> std::future<decltype(func(args...))>;
void wait_all_done();
void wait_and_destroy();
size_t current_num_threads_working() const;
```

用法参考 src/main.cpp