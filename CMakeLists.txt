#############################################################################
# Name:        CMakeListst.txt
# Purpose:     i18n-check
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2023 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
SET(CMAKE_CXX_STANDARD 17)

PROJECT(i18n-check)

ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

SET(FILES src/main.cpp src/i18n_string_util.cpp src/i18n_review.cpp
          src/cpp_i18n_review.cpp src/rc_file_review.cpp)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(i18n-check ${FILES})

# set up compiler flags
IF(MSVC)
  TARGET_COMPILE_OPTIONS(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus /MP /W3 /WX
                         $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2>)
  TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
ELSE()
  TARGET_COMPILE_OPTIONS(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Werror
                         $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
ENDIF()