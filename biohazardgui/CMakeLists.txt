# __________________________________________
# __ guibazard

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)

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
