
project(MyEngine3D)


# Create toolkit source files list
FILE(GLOB MY_ENGINE3D_SRC *.cpp)

add_executable(MyEngine3D ${MY_ENGINE3D_SRC})

target_link_libraries(MyEngine3D PUBLIC bh3d)
target_include_directories(MyEngine3D PUBLIC ${PROJECT_SOURCE_DIR}/biohazard3d/include)