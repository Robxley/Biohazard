# __________________________________________
# __ Library : Biohazard module project - bhmod

project(biohazardmod)

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

# Threads
find_package(Threads REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED)

# __________________________________________
# library

# Create toolkit source files list
FILE(GLOB LOCAL_SOURCEFILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/BHM_*.cpp)


add_library(bhmod 
				STATIC 
					${CMAKE_CURRENT_SOURCE_DIR} 
					${LOCAL_SOURCEFILES})

target_link_libraries(bhmod 
						PUBLIC ${OpenCV_LIBS})

target_include_directories(bhmod 
							PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}/include
			   					${OpenCV_INCLUDE_DIRS})