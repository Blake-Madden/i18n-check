#############################################################################
# Name:        CMakeListst.txt
# Purpose:     i18n-check
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2024 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

project(i18n-check)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(FILES src/main.cpp src/i18n_string_util.cpp src/i18n_review.cpp
          src/cpp_i18n_review.cpp src/rc_file_review.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(i18n-check ${FILES})

# set up compiler flags
if(MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus /MP /W3 /WX
                         $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2>)
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Werror
                         $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
endif()
