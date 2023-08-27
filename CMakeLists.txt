cmake_minimum_required(VERSION 3.16)

# Set your project name here
project(cvm VERSION 1.0.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add an INTERFACE pseudo-library target
add_library(${PROJECT_NAME} INTERFACE)

# Specify include directories
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 'make install' to the correct locations
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match)
install(FILES cmake/${PROJECT_NAME}Config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# This makes the project importable from the build directory
export(EXPORT ${PROJECT_NAME}Targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
)

# Include all the .h files in the include directory
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "include/*.hpp")
add_custom_target(${PROJECT_NAME}_IDE SOURCES ${HEADER_LIST})

if (BUILD_TESTS)
  message(STATUS "Build tests: yes")
  enable_testing()
  add_subdirectory(test)
else()
  message(STATUS "Build tests: no")
endif (BUILD_TESTS)

if (BUILD_BENCHMARKS)
  message(STATUS "Build benchmarks: yes")
  add_subdirectory(benchmark)
else()
  message(STATUS "Build benchmarks: no")
endif (BUILD_BENCHMARKS)
