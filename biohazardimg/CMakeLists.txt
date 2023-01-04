# __________________________________________
# __ Library : Biohazard image project - bhimg

project(biohazardimg)
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

set(BHIMG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

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
        ${CMAKE_CURRENT_SOURCE_DIR}/BHIM_*.cpp)


add_library(bhimg 
				STATIC 
					${CMAKE_CURRENT_SOURCE_DIR} 
					${LOCAL_SOURCEFILES})

target_link_libraries(bhimg 
						PUBLIC ${OpenCV_LIBS})

target_include_directories(bhimg 
							PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR}
			   					${OpenCV_INCLUDE_DIRS})