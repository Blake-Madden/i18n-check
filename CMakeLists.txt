cmake_minimum_required(VERSION 3.9.1)
project(i18n-check)

set(CMAKE_CXX_STANDARD 17)

set(FILES src/main.cpp src/i18n_string_util.cpp src/i18n_review.cpp src/cpp_i18n_review.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(TARGET_NAME i18n-check)
add_executable(i18n-check ${FILES})

# set up compiler flags
if(MSVC)
  target_compile_options(${TARGET_NAME} PRIVATE /W3)
  target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()