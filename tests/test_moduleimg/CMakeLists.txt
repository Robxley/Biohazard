# App - MyModuleImg

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyModuleImg ${LOCAL_FILE_SRC})

target_include_directories(MyModuleImg 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardmod/include
                                ${PROJECT_SOURCE_DIR}/biohazardmimg)

target_link_libraries(MyModuleImg 
                        PUBLIC 
                            bhmod bhimg)

if (WIN32)
    target_compile_options(MyModuleImg PRIVATE /W3 /WX)
else()
    target_compile_options(MyModuleImg PRIVATE -w)
endif()