# App - MyModuleGui

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyModuleTest ${LOCAL_FILE_SRC})

target_include_directories(MyModuleTest 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardmod)

target_link_libraries(MyModuleTest 
                        PUBLIC 
                            biobazardmod)

if (WIN32)
    target_compile_options(MyModuleTest PRIVATE /W3 /WX)
else()
    target_compile_options(MyModuleTest PRIVATE -w)
endif()