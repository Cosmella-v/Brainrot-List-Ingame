cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
project(BrainrotList VERSION 1.0.0)
project(BrainrotList LANGUAGES CXX)
set(WEBVIEW_USE_BUILTIN_MSWEBVIEW2,)
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

# Add WebView library using CPM
#if (EXISTS "${CMAKE_SOURCE_DIR}/webview")
#    CPMAddPackage(
#        NAME webview
#        SOURCE_DIR "${CMAKE_SOURCE_DIR}/webview"
#    )
#else()
#    CPMAddPackage(
#        NAME webview
#        GIT_REPOSITORY https://github.com/webview/webview
#        GIT_TAG 0.12.0
#    )
#    message(STATUS "Added from github")
#endif()
#target_link_libraries(${PROJECT_NAME} webview::core)

# Setup Geode mod
setup_geode_mod(${PROJECT_NAME})
