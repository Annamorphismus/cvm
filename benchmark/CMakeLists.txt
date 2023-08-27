cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../cmake/benchmark.cmake)
include(../cmake/flags.cmake)

# needed for multithreading
find_package(Threads REQUIRED)

add_executable(benchmarks
  benchmark_main.cpp
)

set_flags(benchmarks)
setup_linker(benchmarks)

target_link_libraries(benchmarks LINK_PRIVATE
  ${CMAKE_THREAD_LIBS_INIT}
  benchmark
)

target_include_directories(
  benchmarks PUBLIC
  benchmark
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

add_dependencies(benchmarks benchmark-project)

add_custom_target(bench
  COMMAND  "${CMAKE_BINARY_DIR}/benchmark/benchmarks"
  DEPENDS  benchmarks
)
