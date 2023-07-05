cmake_minimum_required(VERSION 3.17)

project(bpew)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

find_package(SDL2 REQUIRED)	
find_package(assimp REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)

include_directories(${SDL2_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} "include" "include/glm" "include/imgui" "include/entt/single_include" "include/openssl")

FILE(GLOB IMGUISRC "include/imgui/*.cpp" "include/imgui/misc/cpp/*.cpp" "include/imgui/backends/imgui_impl_sdl2.cpp" "include/imgui/backends/imgui_impl_opengl3.cpp")
FILE(GLOB IMGUIZMOSRC "include/ImGuizmo/*.cpp")
FILE(GLOB IMGUIFILEDIALOGSRC "include/ImGuiFileDialog/ImGuiFileDialog.cpp")
FILE(GLOB IMNODESSRC "include/imnodes/*.cpp")
FILE(GLOB OPENSSLSRC "include/openssl/ssl/*.c")
FILE(GLOB GLADSRC "src/glad.c")

add_executable(${PROJECT_NAME} ${IMGUISRC} ${IMGUIZMOSRC} ${IMGUIFILEDIALOGSRC} ${IMNODESSRC} ${GLADSRC} "src/main.cpp" "src/graphics/shaders/shader.cpp" "src/graphics/texture.cpp" "src/graphics/framebuffer.cpp" "src/graphics/index_buffer.cpp" "src/graphics/vertex_buffer.cpp" "src/graphics/vertex_array.cpp" "src/core/window.cpp" "src/core/input.cpp" "src/graphics/renderer.cpp" "src/editor/editor_camera.cpp" "src/graphics/model.cpp" "src/ecs/scene.cpp" "src/editor/editor.cpp" "src/core/time.cpp" "src/graphics/gizmos.cpp" "src/nodes/node.cpp" "src/nodes/node_link.cpp" "src/editor/space_layout.cpp" "src/editor/space_modeling.cpp" "src/editor/space_shading.cpp" "src/graphics/shaders/shader_builder.cpp" "src/graphics/shaders/shader_parser.cpp" "src/core/tokenizer.cpp" "src/editor/event_receiver.cpp")

link_directories(${ASSIMP_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARIES} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARIES} OpenSSL::SSL)
