#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Cuneiform GUI
# Author:      Blake Madden
# Created:     2024-08-17
# Copyright:   (c) 2021-2024 Blake Madden
# License:     3-Clause BSD license
#############################################################################

# Declare the minimum required CMake version
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
elseif(UNIX)
    set(UNIX_ARCHITECTURE "x86_64" CACHE STRING "" FORCE)
endif()

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "macOS Deployment Target")
    endif()
    message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

project(cuneiform VERSION 2024.0.0.0 DESCRIPTION "Cuneiform")
set(PROGRAM_NUMBERED_VERSION 1.0)
set(PROGRAM_AUTHOR "Blake Madden")
set(PROGRAM_COMMENTS "Internationalization/localization analysis")
set(PROGRAM_RDNS_IDENTIFIER "org.cuneiform.cuneiform")
set(PROGRAM_LICENSE "BSD-3-Clause")

# Build a displayable version string that only includes the necessary parts
set(FULL_DISPLAY_VERSION ${cuneiform_VERSION_MAJOR})
if(NOT ${cuneiform_VERSION_MINOR} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${cuneiform_VERSION_MINOR}")
endif()
if(NOT ${cuneiform_VERSION_PATCH} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${cuneiform_VERSION_PATCH}")
endif()
if(NOT ${cuneiform_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${cuneiform_VERSION_TWEAK}")
endif()
string(TIMESTAMP BUILD_YEAR "%Y")

message(STATUS "${cuneiform_DESCRIPTION}")
message(STATUS "${PROGRAM_COMMENTS}")
message(STATUS "Full version: ${cuneiform_VERSION_MAJOR}.${cuneiform_VERSION_MINOR}.${cuneiform_VERSION_PATCH}.${cuneiform_VERSION_TWEAK}")
message(STATUS "Display version: ${FULL_DISPLAY_VERSION}")
message(STATUS "Numbered Version: ${PROGRAM_NUMBERED_VERSION}")
message(STATUS "Built in: ${BUILD_YEAR}")
message(STATUS "Authored by: ${PROGRAM_AUTHOR}")
message(STATUS "Published by: ${PUBLISHER}")
message(STATUS "Homepage: ${cuneiform_HOMEPAGE_URL}")
message(STATUS "Output program file: ${CMAKE_PROJECT_NAME}")
message(STATUS "Writing program info to build peripheral files")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resource.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")

# Build the documentation
#########################
find_package(Doxygen)
if(TARGET doxygen-docs)
    if(NOT DOXYGEN_FOUND)
        message(STATUS "Doxygen not found. API documentation cannot be generated.")
    endif()
endif()

add_custom_target(doxygen-docs
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/../docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../docs/doxygen"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

# Setup wxWidgets (can be 1-5 folders up from here)
########################
include(../cmake/folders.cmake)

# Find wxWidgets (wxWidgetsConfig.cmake, specifically) and select its components
find_directory("wxWidgets" wxWidgets_BASEDIR)
if(WIN32)
    set(wxWidgets_DIR "${wxWidgets_BASEDIR}/out/install/x64-${CMAKE_BUILD_TYPE}/lib/cmake/wxWidgets")
elseif(UNIX)
    set(wxWidgets_DIR "${wxWidgets_BASEDIR}/wxlib/lib/cmake/wxWidgets")
endif()

message(STATUS "Adding wxWidgets CMake config folder: ${wxWidgets_DIR}")
if(NOT EXISTS "${wxWidgets_DIR}/wxWidgetsConfig.cmake" AND NOT EXISTS "${wxWidgets_DIR}/wxwidgets-config.cmake")
    message(WARNING "Cannot find wxWidgets CMake configuration. (Looked in ${wxWidgets_DIR}.)")
endif()
set(wxWidgets_USE_STATIC 1)
find_package(wxWidgets 3.3 COMPONENTS core base xrc propgrid xml ribbon qa net stc richtext REQUIRED CONFIG)
message(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")
message(STATUS "Adding wxWidgets: ${wxPLATFORM_LIB_DIR}")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build the app
########################
set(FILES ../src/analyze.cpp ../src/gui/i18napp.cpp ../src/gui/projectdlg.cpp ../src/i18n_string_util.cpp
          ../src/i18n_review.cpp ../src/input.cpp ../src/gui/datamodel.cpp ../src/gui/app_options.cpp
          ../src/cpp_i18n_review.cpp ../src/csharp_i18n_review.cpp  ../src/po_file_review.cpp
          ../src/translation_catalog_review.cpp ../src/rc_file_review.cpp
          ../src/pseudo_translate.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    set(wxUSE_DPI_AWARE_MANIFEST 1)
    list(APPEND FILES ${CMAKE_CURRENT_SOURCE_DIR}/cuneiform.rc)
    list(APPEND FILES "${wxWidgets_BASEDIR}/include/wx/msw/wx_dpi_aware.manifest")
elseif(APPLE)
    # Add an icon for the apple .app file
    list(APPEND FILES ${CMAKE_SOURCE_DIR}/app-logo.icns)
endif()

# Define the build target for the executable
message(STATUS "Configuring program...")
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${FILES})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ _UNICODE
    $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211
                            $<$<CONFIG:Debug>:/Od>
                            $<$<CONFIG:Release>:/O2>)
elseif(MINGW OR MSYS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ _UNICODE
            $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXOSX__
        $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__
        $<$<CONFIG:Debug>:__WXDEBUG__> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og -g3>
                           $<$<CONFIG:Release>:-O2>)
endif()

# Link required libraries to the executable
if(WIN32 OR APPLE)
    # AppleClang dropped support for OpenMP
    # Including TBB is not necessary for macOS and actually breaks hardened runtime
    # because that library is not included by default with the OS (XCode installs it)
    # and it is not signed.
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
elseif(UNIX)
    # TBB is required for GCC linkage sometimes, although this app is not explicitly using it,
    # so try to link it in.
    find_package(TBB)
    if(TBB_FOUND)
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb)
    else()
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    endif()
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE app-logo.icns
        # call the app bundle Cuneiform (uppercase)
        OUTPUT_NAME "Cuneiform"
        XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
endif()

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/../docs/manual/book/i18n-check.pdf $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/images $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/images)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/../app-logo.svg $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/images)
