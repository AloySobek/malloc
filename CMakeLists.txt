cmake_minimum_required(VERSION 3.18)

project(
  malloc
  VERSION 0.1.0
  LANGUAGES C)

include(CTest)
include(FetchContent)

option(BUILD_TESTS "" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

FetchContent_Declare(
  check
  GIT_REPOSITORY https://github.com/libcheck/check.git
  GIT_TAG 11970a7 # 0.15.2
)

FetchContent_MakeAvailable(check)

file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE TEST_SRC "${CMAKE_SOURCE_DIR}/test/*.c")

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(first ${TEST_SRC})

target_include_directories(first PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(first PUBLIC ${check_BINARY_DIR})
target_include_directories(first PUBLIC ${check_BINARY_DIR}/src)
target_include_directories(first PUBLIC ${check_SOURCE_DIR}/src)

target_link_libraries(first PRIVATE ${PROJECT_NAME})
target_link_libraries(first PRIVATE check)

add_test(NAME first COMMAND first)
