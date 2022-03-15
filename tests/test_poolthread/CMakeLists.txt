# App - MyModuleGui

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyThreadPoolTest ${LOCAL_FILE_SRC})

target_include_directories(MyThreadPoolTest 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardmod/include)

target_link_libraries(MyThreadPoolTest 
                        PUBLIC 
                            bhmod)

if (WIN32)
    target_compile_options(MyThreadPoolTest PRIVATE /W3 /WX)
else()
    target_compile_options(MyThreadPoolTest PRIVATE -w)
endif()