cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(../cmake/gtest.cmake)
include(../cmake/flags.cmake)

function (new_test source name)
  add_executable(${name} ${source})
  target_link_libraries(${name} LINK_PUBLIC
    ${CMAKE_THREAD_LIBS_INIT}
    gtest
    gtest_main
  )

  target_include_directories(${name} PUBLIC
    gtest
    gtest_main
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )

  set_flags(${name})
  set_sanitizers(${name})
  setup_linker(${name})

  include(GoogleTest)
  gtest_discover_tests(${name})

  add_dependencies(${name} gtest-project)
endfunction()

new_test(test_treap.cpp test_treap)
