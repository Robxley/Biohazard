# __________________________________________
# __ Library : Biohazard 3d project - bh3d 

project(biohazard3d)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Biohazard3d Build options: ${CMAKE_CXX_FLAGS}")

if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

# __________________________________________
# 3rd libraries / Packages

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

FILE(GLOB IMGUI_SOURCEFILES
        ${IMGUI_DIR}/*.cpp)

# __________________________________________
# library

FILE(GLOB BH3D_SOURCEFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BH3D_*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/BH3D_*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/cv/BH3D_*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/std/BH3D_*.cpp)

if(WIN32)
	set(WIN32_DEPENDENCIES_LIBRARIES GLEW::GLEW SDL2::SDL2 SDL2::SDL2main)
endif()

add_library(bh3d STATIC ${BH3D_SOURCEFILES} ${IMGUI_SOURCEFILES})

set_target_properties(bh3d PROPERTIES
                    	OUTPUT_NAME bh3d
                    	LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
                    	ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

target_include_directories(bh3d
							PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/include
								${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
								${CMAKE_CURRENT_SOURCE_DIR}/include/cv
								${CMAKE_CURRENT_SOURCE_DIR}/include/std
								${IMGUI_DIR}
							SYSTEM PUBLIC
								${glm_INCLUDE_DIR}
							PRIVATE
								${OPENGL_INCLUDE_DIRS}
							   	${SDL2_INCLUDE_DIRS}
							   	${OpenCV_INCLUDE_DIRS}
								${GLEW_INCLUDE_DIRS})

target_link_libraries(bh3d
						PRIVATE 
							${OpenCV_LIBS}
							${SDL2_LIBRARIES}
							${OPENGL_LIBRARIES}
							${GLEW_LIBRARIES}
							${WIN32_DEPENDENCIES_LIBRARIES})
if (WIN32)
	target_compile_options(bh3d PRIVATE /W3 /WX /utf-8 /Zc:char8_t- /MP)
else()
	target_compile_options(bh3d PRIVATE -w)
endif()
