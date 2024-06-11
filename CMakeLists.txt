cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message("Setting default build type to Debug")
endif()

project(thread_pool_demo VERSION 0.0.1 LANGUAGES C CXX)

# header file directory
#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(./test)


# ======================================= src =============================================

# 收集所有不是 main 的源文件 .cc .c .cpp 以便在构建库时仅包含其他源文件
file(GLOB_RECURSE ALL_SRC "src/*.cpp" "src/*.cc" "src/*.c")
set(MAIN_SRC src/main.cpp)
message("main src: " ${MAIN_SRC})

# 如果当前源文件不是主文件，则将其添加到 LIB_SRC 变量中
foreach (F ${ALL_SRC})
    if (NOT ${F} STREQUAL ${MAIN_SRC})
        message("source file: " ${F})
        set(LIB_SRC ${LIB_SRC} ${F})
    endif()
endforeach (F)

# 指定可执行文件, 库文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)       # ./build/bin
message("Binary directory:" ${EXECUTABLE_OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)          # ./build/lib
message("Archive directory:" ${LIBRARY_OUTPUT_PATH})

# 定义可执行文件目标, src下只有main.cpp可执行
add_executable(main ${MAIN_SRC})

# 定义静态库，包含所有 src 下的源文件 + 外部静态库
ADD_LIBRARY(my_static_lib STATIC ${LIB_SRC})

# 链接刚定义的静态库到可执行文件
target_link_libraries(main my_static_lib)

# 在这里添加一些第三方通过 make install 的库
find_library(LIBURING_LIBRARY NAMES uring)
target_link_libraries(main ${LIBURING_LIBRARY})

# 添加编译选项: -fpermissive: 忽略精度丢失警告
target_compile_options(main PRIVATE -fpermissive)

# 添加 pthread 库
set(CMAKE_CXX_FLAGS -pthread)
TARGET_LINK_LIBRARIES(main pthread dl)

# 用于头文件自定义的选项
add_compile_definitions(THPOOL_DEBUG)

# c++20 coroutine
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++20 -fcoroutines")
