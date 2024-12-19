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

# check the coreeeeeee
if (NOT TARGET webview::core)
    CPMAddPackage(
        NAME webview
        GIT_REPOSITORY https://github.com/webview/webview
        GIT_TAG 0.12.0
    )
    if (webview_ADDED)
        target_link_libraries(${PROJECT_NAME} webview::core)
        message(STATUS "added webview package!, :3")
    else()
        message(FATAL_ERROR "Failed to add WebView package! FUCK YOU")
    endif()
else()
    message(STATUS "WebView already added, skipping. PLEASE FIX")
endif()


# Setup Geode mod
setup_geode_mod(${PROJECT_NAME})
