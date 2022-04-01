# App - HsiViewer

# Create toolkit source files list
FILE(GLOB LOCAL_FILE_SRC *.cpp)

add_executable(HsiViewer ${LOCAL_FILE_SRC})

target_include_directories(HsiViewer 
                            PUBLIC
                                ${PROJECT_SOURCE_DIR}/biohazardgui
                                ${PROJECT_SOURCE_DIR}/biohazardimg)

target_link_libraries(HsiViewer 
                        PUBLIC 
                            bhgui
                            bhimg)

if (WIN32)
    target_compile_options(HsiViewer PRIVATE /W3 /WX /utf-8 /Zc:char8_t- /MP)
else()
    target_compile_options(HsiViewer PRIVATE -w)
endif()