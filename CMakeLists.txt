cmake_minimum_required(VERSION 3.9.1)
project(i18n-check)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(FILES src/main.cpp src/i18n_string_util.cpp src/i18n_review.cpp src/cpp_i18n_review.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(i18n-check ${FILES})