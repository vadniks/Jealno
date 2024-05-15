
cmake_minimum_required(VERSION 3.26)
project(Jealno LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Wpedantic")
add_compile_options("-O3")
#add_link_options("-s")

add_compile_options("-Wno-c99-extensions")
add_compile_options("-Wno-vla-extension")

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf SDL2_image GL GLEW assimp)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
