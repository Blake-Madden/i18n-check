#############################################################################
# Name:        CMakeListst.txt
# Purpose:     Unit test runner i18n-check
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2023 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

PROJECT(i18nTestRunner)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

# place Catch2 at the same folder level as this repo if it isn't installed
# (you will need to do this on Windows or macOS or if version 3 of Catch2 isn't installed)
GET_FILENAME_COMPONENT(_fullpath "${_dir}" REALPATH)
IF(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../../Catch2")
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
ELSE()
    # ...otherwise, see if it is installed
    FIND_PACKAGE(Catch2 3 REQUIRED)
ENDIF()

ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ../src/i18n_string_util.cpp
../src/i18n_review.cpp ../src/cpp_i18n_review.cpp stringutiltests.cpp)
IF(MSVC)
    TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
ENDIF()
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

# load the test cases into the runner
INCLUDE(CTest)
INCLUDE(Catch)
CATCH_DISCOVER_TESTS(${CMAKE_PROJECT_NAME})
