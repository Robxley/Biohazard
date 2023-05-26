# App - MyModuleCap

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyModuleCap ${LOCAL_FILE_SRC})

target_include_directories(MyModuleCap 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardgui)

target_link_libraries(MyModuleCap 
                        PUBLIC 
                            bhgui
                            user32)

if(WIN32)
    target_link_libraries(MyModuleCap 
                            PUBLIC 
                                bhgui 
                                user32)
    target_compile_options(MyModuleCap PRIVATE /W3 /WX)
else()
    target_link_libraries(MyModuleCap 
                            PUBLIC 
                                bhgui)
    target_compile_options(MyModuleCap PRIVATE -w)
endif()