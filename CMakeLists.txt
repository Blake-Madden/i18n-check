#############################################################################
# Name:        CMakeListst.txt
# Purpose:     Cuneiform
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2024 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

project(cuneiform)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Release")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(FILES src/analyze.cpp src/main.cpp src/i18n_string_util.cpp src/i18n_review.cpp src/input.cpp
          src/translation_catalog_review.cpp src/pseudo_translate.cpp
          src/cpp_i18n_review.cpp src/csharp_i18n_review.cpp
          src/po_file_review.cpp src/rc_file_review.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(cuneiform ${FILES})

# set up compiler flags
if(MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus /MP /W3 /WX
                         $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2>)
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Werror
                         $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
endif()

if(UNIX AND NOT APPLE)
    find_package(TBB)
    if(TBB_FOUND)
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${CMAKE_PROJECT_NAME} TBB::tbb)
    else()
        target_link_libraries(${CMAKE_PROJECT_NAME})
    endif()
endif()
