# __________________________________________
# biohazard3d project

project(bh3d)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Display build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

# __________________________________________
# 3rd libraries / Packages

# Threads
find_package(Threads REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# glad or glew
if(NOT GLAD_SRC_DIRS)
	find_package(glad CONFIG QUIET)
	if (glad_FOUND)
		   include_directories(${GLAD_INCLUDE_DIRS})
		   link_libraries(${GLAD_LIBRARIES})
	else()
		find_package(GLEW CONFIG REQUIRED)
		if (GLEW_FOUND)
			   include_directories(${GLEW_INCLUDE_DIRS})
			   link_libraries(${GLEW_LIBRARIES})
		endif()
	endif()
endif()

# GLM
find_package(glm CONFIG QUIET)
if (NOT ${glm_FOUND})
    include_directories("${PROJECT_SOURCE_DIR}/../3rd")
endif()

# SLD2 
find_package(SDL2 CONFIG REQUIRED)


# OpenCV
find_package(OpenCV QUIET)


# __________________________________________
# Create source files list

# Main biohazard3d source file
FILE(GLOB BH3D_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BH3D*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/std/*.cpp)
		
# Inlucde imgui source file if valable
if(IMGUI_DIR)
	FILE(GLOB BH3D_IMGUI
        ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/*.cpp
		${IMGUI_DIR}/*.cpp)

	set(BH3D_SRC ${BH3D_SRC} ${BH3D_IMGUI} ${IMGUI_SOURCEFILES})
endif()

# Inlucde Opencv source file if valable
if(OpenCV_FOUND)
	FILE(GLOB BH3D_OPENCV 
		${CMAKE_CURRENT_SOURCE_DIR}/src/cv/*.cpp)
		
	set(BH3D_SRC ${BH3D_SRC} ${BH3D_OPENCV})
endif()


set(DEPENDENCIES_LIBRARIES glad::glad SDL2::SDL2 SDL2::SDL2main glm)
message(STATUS "DEPENDENCIES_LIBRARIES: ${DEPENDENCIES_LIBRARIES}")


# __________________________________________
# Biohazard3d library

add_library(bh3d STATIC ${CMAKE_CURRENT_SOURCE_DIR} ${BH3D_SRC})

target_link_libraries(bh3d PUBLIC ${OpenCV_LIBS} ${DEPENDENCIES_LIBRARIES} ${OPENGL_LIBRARIES})

target_include_directories(bh3d 
							PUBLIC
							${OPENGL_INCLUDE_DIRS}
                           	${SDL2_INCLUDE_DIRS}
			   				${OpenCV_INCLUDE_DIRS}
			   				${CMAKE_CURRENT_SOURCE_DIR}/include
							${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
							${CMAKE_CURRENT_SOURCE_DIR}/include/cv
							${CMAKE_CURRENT_SOURCE_DIR}/include/std
			   				${3RD_DIR}/imgui)

