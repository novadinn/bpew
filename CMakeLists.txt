cmake_minimum_required(VERSION 3.17)

project(bpew)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

find_package(SDL2 REQUIRED)
find_package(assimp REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIR} "include" "include/glm" "include/imgui" "include/entt/single_include")

FILE(GLOB IMGUISRC "include/imgui/*.cpp" "include/imgui/misc/cpp/*.cpp" "include/imgui/backends/imgui_impl_sdl2.cpp" "include/imgui/backends/imgui_impl_opengl3.cpp")

add_executable(${PROJECT_NAME} ${IMGUISRC} "src/glad.c" "src/main.cpp" "src/graphics/shader.cpp" "src/graphics/texture.cpp" "src/graphics/framebuffer.cpp" "src/graphics/index_buffer.cpp" "src/graphics/vertex_buffer.cpp" "src/graphics/vertex_array.cpp" "src/core/window.cpp" "src/core/input.cpp" "src/core/file_system.cpp" "src/graphics/renderer.cpp" "src/graphics/camera.cpp" "src/graphics/model.cpp" "src/ecs/scene.cpp")

link_directories(${ASSIMP_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARIES} GL dl)
