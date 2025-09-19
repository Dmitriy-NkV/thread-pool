cmake_minimum_required(VERSION 3.23.1)
project(ThreadPool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(thread-pool
  src/main.cpp
  src/thread_pool.cpp
  src/utils/logger.cpp
)

target_include_directories(thread-pool PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
)
