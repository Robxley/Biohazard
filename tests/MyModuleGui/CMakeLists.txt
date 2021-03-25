# App - MyModuleGui

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyModuleGui ${LOCAL_FILE_SRC})

target_include_directories(MyModuleGui 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardgui)

target_link_libraries(MyModuleGui 
                        PUBLIC 
                            bhgui)

if (WIN32)
    target_compile_options(MyModuleGui PRIVATE /W3 /WX)
else()
    target_compile_options(MyModuleGui PRIVATE -w)
endif()