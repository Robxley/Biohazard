# __________________________________________
# modulazard project

project(biobazardmod)

# Set C++ standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

# __________________________________________
# 3rd libraries / Packages

# Threads
find_package(Threads REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED)

# __________________________________________
# library

# Create toolkit source files list
FILE(GLOB LOCAL_SOURCEFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BHM_*.cpp)


add_library(biobazardmod 
				STATIC 
					${CMAKE_CURRENT_SOURCE_DIR} 
					${LOCAL_SOURCEFILES})

target_link_libraries(biobazardmod 
						PUBLIC ${OpenCV_LIBS})

target_include_directories(biobazardmod 
							PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/include
			   					${OpenCV_INCLUDE_DIRS})