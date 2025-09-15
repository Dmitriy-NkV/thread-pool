cmake_minimum_required(VERSION 3.15)
project(TodoApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(thread-pool
  src/main.cpp
  src/thread_pool.cpp
)

target_include_directories(thread-pool PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
