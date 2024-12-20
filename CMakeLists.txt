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


CPMAddPackage(
    NAME webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 0.12.0
)
message(STATUS "Added from github")
if (NOT DEFINED ENV{GITHUB_ACTIONS})
# Link WebView library
target_link_libraries(${PROJECT_NAME} webview::core)
endif()

# Setup Geode mod
setup_geode_mod(${PROJECT_NAME})
