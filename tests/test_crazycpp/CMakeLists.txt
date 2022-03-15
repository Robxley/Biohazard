# App - MyCrazyCppTests

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(MyCrazyCppTests ${LOCAL_FILE_SRC})

target_include_directories(MyCrazyCppTests 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR})

if (WIN32)
    target_compile_options(MyCrazyCppTests PRIVATE /W3 /WX)
else()
    target_compile_options(MyCrazyCppTests PRIVATE -w)
endif()