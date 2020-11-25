cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

# Set project name
project(Biobazard)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# __________________________________________
# __ External libraries


#3rd Directory
set(3RD_DIR ${PROJECT_SOURCE_DIR}/3rd)
message(STATUS "Third dependency directory: ${3RD_DIR}")

# imgui
set(IMGUI_DIR ${3RD_DIR}/imgui)

# Add subdirectories containing CMakeLists files
add_subdirectory(biohazard3d)
add_subdirectory(tests)