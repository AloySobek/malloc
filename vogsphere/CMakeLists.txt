cmake_minimum_required(VERSION 3.18)

if(NOT DEFINED ENV{HOSTTYPE})
  execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE UNAME_M
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND uname -s
    OUTPUT_VARIABLE UNAME_S
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(ENV_HOSTTYPE "${UNAME_M}_${UNAME_S}")

  set(ENV{HOSTTYPE} ${ENV_HOSTTYPE})
endif()

message("HOSTTYPE: $ENV{HOSTTYPE}")

project(
  "ft_malloc_$ENV{HOSTTYPE}"
  VERSION 0.1.0
  LANGUAGES C)

option(LINK "" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(benchmark ${CMAKE_SOURCE_DIR}/benchmark.c)

target_include_directories(benchmark PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(LINK)
  target_link_libraries(benchmark PRIVATE ${PROJECT_NAME})
endif()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:${PROJECT_NAME}>
          ${CMAKE_BINARY_DIR}/libft_malloc.dylib)
