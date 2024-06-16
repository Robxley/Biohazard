# __________________________________________
# __ Executable

# Create toolkit source files list


FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyEngine3D ${LOCAL_FILE_SRC})

target_include_directories(MyEngine3D 
                            SYSTEM PRIVATE
                                ${glm_INCLUDE_DIR}
                            PUBLIC 
								${IMGUI_DIR}
								${IMGUI_DIR}/backends
                                ${CMAKE_CURRENT_SOURCE_DIR}
								${GLEW_INCLUDE_DIRS}
                                ${PROJECT_SOURCE_DIR}/biohazard3d/include)

target_link_libraries(MyEngine3D PUBLIC bh3d)

if (WIN32)
    target_compile_options(MyEngine3D PRIVATE /W3 /WX)
else()
    target_compile_options(MyEngine3D PRIVATE -w)
endif()