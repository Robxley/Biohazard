# __________________________________________
# __ Executable

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyEngine2D ${LOCAL_FILE_SRC})

target_include_directories(MyEngine2D 
                            SYSTEM PRIVATE
                                ${glm_INCLUDE_DIR}
                            PRIVATE 
                                ${CMAKE_CURRENT_SOURCE_DIR}
								${GLEW_INCLUDE_DIRS}
                                ${PROJECT_SOURCE_DIR}/biohazard3d/include
                                ${PROJECT_SOURCE_DIR}/biohazardgui/include)

target_link_libraries(MyEngine2D PRIVATE bh3d bhgui)

if (WIN32)
    target_compile_options(MyEngine2D PRIVATE /W3 /WX)
else()
    target_compile_options(MyEngine2D PRIVATE -w)
endif()