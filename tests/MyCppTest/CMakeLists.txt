# App - MyModuleGui

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyCppTest ${LOCAL_FILE_SRC})

target_include_directories(MyCppTest 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardmod/include)

target_link_libraries(MyCppTest 
                        PUBLIC 
                            bhmod)

if (WIN32)
    target_compile_options(MyCppTest PRIVATE /W3 /WX)
else()
    target_compile_options(MyCppTest PRIVATE -w)
endif()