cmake_minimum_required(VERSION 3.18)

project(
  malloc
  VERSION 0.1.0
  LANGUAGES C)

option(LINK "" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#add_compile_options(-Wall -Wextra -Werror)

file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(benchmark ${CMAKE_SOURCE_DIR}/benchmark.c)

target_include_directories(benchmark PUBLIC ${CMAKE_SOURCE_DIR}/src)

if (LINK)
    target_link_libraries(benchmark PRIVATE ${PROJECT_NAME})
endif()
