# __________________________________________
# __ Library : Biohazard gui project - bhgui

project(biohazardgui)
message(STATUS "🔶 Project: ${PROJECT_NAME}")

# Set C++ standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    add_compile_options(/utf-8 /Zc:char8_t-)
endif()

if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

# __________________________________________
# 3rd libraries / Packages

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)

# __________________________________________
# library

# Create toolkit source files list
FILE(GLOB IMGUI_SOURCEFILES
        ${IMGUI_DIR}/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/imgui_extra/*.cpp)

# Create toolkit source files list
FILE(GLOB LOCAL_SOURCEFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/BHG_*.cpp)


# __________________________________________
# __ Executable

if(WIN32)
	set(WIN32_DEPENDENCIES_LIBRARIES GLEW::GLEW SDL2::SDL2 SDL2::SDL2main)
endif()

add_library(bhgui STATIC ${LOCAL_SOURCEFILES} ${IMGUI_SOURCEFILES})

target_link_libraries(bhgui 
						PUBLIC 
							${OpenCV_LIBS} 
							${SDL2_LIBRARIES}
							${WIN32_DEPENDENCIES_LIBRARIES})

target_compile_definitions(bhgui 
							PUBLIC 
								IMGUI_SHOW_DEMO=0)

target_include_directories(bhgui 
							PUBLIC
			   					${OpenCV_INCLUDE_DIRS}
								${CMAKE_CURRENT_SOURCE_DIR}/imgui_extra
								${IMGUI_DIR})
