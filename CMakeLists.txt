cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

# Set project name and version
project(Biohazard VERSION "0.1.0" LANGUAGES CXX)

# Set default runtime directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Add CMake modules folder path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /Zc:char8_t- /MP")
    message(STATUS "Build options: ${CMAKE_CXX_FLAGS}")
endif()

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


# Display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Copy ressources to default runtime directory
file(GLOB resources ${PROJECT_SOURCE_DIR}/3rd/resources/*)
file(COPY ${resources} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "Resources were copied to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# __________________________________________
# __ Libraries

# Library options
#set(OpenGL_GL_PREFERENCE GLVND)

# External libraries
#find_package(Threads REQUIRED)
#find_package(OpenCV REQUIRED)
#find_package(OpenGL REQUIRED)
#find_package(SDL2 REQUIRED)
#find_package(GLAD REQUIRED)

#3rd Directory
set(3RD_DIR ${PROJECT_SOURCE_DIR}/3rd)
message(STATUS "Third dependency directory: ${3RD_DIR}")

# Header only libraries
#set(spdlog_INCLUDE_DIR ${3RD_DIR}/spdlog/include)		# spdlog - Fast C++ logging library
#set(json_INCLUDE_DIR ${3RD_DIR}/json/include)			# JSON for modern C++ library
#set(glm_INCLUDE_DIR ${3RD_DIR}/glm/include)		    # glm - OpenGL Mathematics
#set(IMGUI_DIR ${3RD_DIR}/imgui)						# ImGui - Dear ImGui
set(ICONS_FONT_AWESOME_DIR ${3RD_DIR}/IconsFontAwesome)						# Incons Font Awesome


# __________________________________________
# __ Subdirectories

add_subdirectory(biohazardmod)
add_subdirectory(biohazard3d)
add_subdirectory(biohazardgui)
add_subdirectory(biohazardimg)
add_subdirectory(tests)