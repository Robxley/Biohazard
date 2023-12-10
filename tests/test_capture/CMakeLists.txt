# App - MyModuleCap

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp *.h)

add_executable(MyModuleCap ${LOCAL_FILE_SRC})

target_include_directories(MyModuleCap 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}
                                ${PROJECT_SOURCE_DIR}/biohazardgui)

target_link_libraries(MyModuleCap 
                        PUBLIC 
                            bhgui
                            user32)

if(WIN32)
    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
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