cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(BrainrotList VERSION 1.0.0)

# Add source files for the project
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/Demon/LevelCell.cpp
    src/Demon/LevelInfoLayer.cpp
    # Add any extra C++ source files here
)

# Add Geode SDK
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Function to check if webview.lib exists in the build folder or its subdirectories
function(check_webview_lib_in_build)
    set(WEBVIEW_LIB_NAME "webview.lib")

    # Search within the build folder and its subdirectories
    file(GLOB_RECURSE CANDIDATE_PATHS RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/*/webview-build")

    if (CANDIDATE_PATHS)
        foreach(PATH ${CANDIDATE_PATHS})
            if (EXISTS "${CMAKE_BINARY_DIR}/${PATH}")
                message(STATUS "Found ${WEBVIEW_LIB_NAME} at ${CMAKE_BINARY_DIR}/${PATH}")
                set(WEBVIEW_FOUND TRUE PARENT_SCOPE)
                set(WEBVIEW_LIB_PATH "${CMAKE_BINARY_DIR}/${PATH}" PARENT_SCOPE)
                return()
            endif()
        endforeach()
    endif()

    set(WEBVIEW_FOUND FALSE PARENT_SCOPE)
endfunction()

# Check if webview.lib exists in the build directory
check_webview_lib_in_build()

# Proceed with adding WebView package if it was not found
if (NOT WEBVIEW_FOUND)
    if (NOT TARGET webview::core)
        message(STATUS "WebView library not found, adding WebView package...")
        CPMAddPackage(
            NAME webview
            GIT_REPOSITORY https://github.com/webview/webview
            GIT_TAG 0.12.0
        )
        if (webview_ADDED)
            target_link_libraries(${PROJECT_NAME} webview::core)
            message(STATUS "WebView package successfully added!")
        else()
            message(FATAL_ERROR "Failed to add WebView package!")
        endif()
    else()
        message(STATUS "WebView already added, skipping...")
    endif()
else()
    message(STATUS "Found webview.lib, skipping WebView package addition.")
endif()

# Setup Geode mod
setup_geode_mod(${PROJECT_NAME})
