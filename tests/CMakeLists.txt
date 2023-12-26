#############################################################################
# Name:        CMakeListst.txt
# Purpose:     Unit test runner i18n-check
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2024 Blake Madden
# License:     3-Clause BSD license
#############################################################################

project(i18nTestRunner)

cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_program(CLANG_TIDY_COMMAND
      NAMES
         clang-tidy
      NO_CACHE)
# This takes a long time to run, so only run clang-tidy
# if explicitly requested
if(CLANG_TIDY_COMMAND AND MSVC AND RUN_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_COMMAND} --extra-arg=/EHsc)
endif()

# place Catch2 at the same folder level as this repo if it isn't installed
# (you will need to do this on Windows or macOS or if version 3 of Catch2 isn't installed)
get_filename_component(_fullpath "${_dir}" REALPATH)
if(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../../Catch2")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
elseif(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../../../Catch2")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
elseif(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../../../../Catch2")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
else()
    # ...otherwise, see if it is installed
    find_package(Catch2 3 REQUIRED)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(${CMAKE_PROJECT_NAME} ../src/i18n_string_util.cpp
../src/i18n_review.cpp ../src/cpp_i18n_review.cpp
../src/rc_file_review.cpp
i18nstringtests.cpp stringutiltests.cpp cpptests.cpp
decodetests.cpp rctests.cpp testingmain.cpp)
if(MSVC)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP /W3 /WX
                           $<$<CONFIG:Debug>:/Od /fsanitize=address> $<$<CONFIG:Release>:/O2>)
else()
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST)
endif()
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Catch2::Catch2)

# load the test cases into the runner
include(CTest)
include(Catch)
catch_discover_tests(${CMAKE_PROJECT_NAME})
