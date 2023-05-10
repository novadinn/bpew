#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "core/file_system.h"
#include "core/window.h"
#include "core/log.h"
#include "core/input.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/index_buffer.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"
#include "graphics/renderer.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/model.h"
#include "graphics/framebuffer.h"
#include "ecs/scene.h"
#include "ecs/entity.h"
#include "ecs/components.h"
#include "editor/editor.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define NUM_SAMPLES 16
#define CLEAR_COLOR glm::vec4{0.2, 0.2, 0.2, 1.0}

const char* line_shader_vs = R"(
#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aColor;

out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    outColor = aColor;
}
)";
const char* line_shader_fs = R"(
#version 460 core

out vec4 fragColor;

in vec3 outColor;

void main() {
    fragColor = vec4(outColor, 1.0);
}
)";

int main(int argc, char** argv) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL\n");
		exit(1);
	}
	SDL_GL_LoadLibrary(NULL);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, NUM_SAMPLES);
	
	Window window;
	if(!window.create("BPew",
					  SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED,
					  WINDOW_WIDTH, WINDOW_HEIGHT,
					  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) {
		exit(1);
	}
	
	SDL_GLContext context = window.createContext();
	if(!context) {
		exit(1);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigDockingWithShift = true;
	
	SDL_Window* native_window = window.getNativeWindow();
	ImGui_ImplSDL2_InitForOpenGL(native_window, context);
	ImGui_ImplOpenGL3_Init("#version 460");
	
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		LOG_ERROR("Failed to initialize GLAD\n");
		exit(1);
    }
	
	SDL_GL_SetSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Renderer::setClearColor(CLEAR_COLOR);
	
	Shader line_shader;
	line_shader.createFromSource(line_shader_vs, line_shader_fs);
	
	float last_time = SDL_GetTicks();

	Editor editor;
	editor.create();
	
	bool running = true;
	while(running) {
		Input::begin();
		
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			
			switch(event.type) {
			case SDL_KEYDOWN: {
				if (!event.key.repeat)
					Input::keyDownEvent(event);
			} break;
			case SDL_KEYUP: {
				Input::keyUpEvent(event);
			} break;
			case SDL_MOUSEBUTTONDOWN: {
				Input::mouseButtonDownEvent(event);
			} break;
			case SDL_MOUSEBUTTONUP: {
				Input::mouseButtonUpEvent(event);
			} break;
			case SDL_MOUSEWHEEL: {
				Input::wheelEvent(event);
			} break;
			case SDL_WINDOWEVENT: {
				if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
					uint32 window_id = event.window.windowID;
					if(window.getID() == window_id) {
						running = false;
					}
				}
			};
			};
		}

		float current_time = (float)SDL_GetTicks() / 1000;
		float delta_time = current_time - last_time;
		last_time = current_time;

		editor.onUpdate();
		
		window.makeContextCurrent(context);
		
		int w, h;
		window.getViewport(&w, &h);
		glViewport(0, 0, w, h);
		
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

		editor.onDraw();
		
		// std::vector<float> line_vertices;
		// const glm::vec3& cam_pos = camera.getPosition();
		// for(float x = cam_pos.x - far; x < cam_pos.x + far; x += 0.5f) {
		// 	glm::vec3 color = {0.4, 0.4, 0.4};
		// 	if((int)x == 0) {
		// 		color = {1, 0.4, 0.4};
		// 	}
			
		// 	line_vertices.push_back((int)x);
		// 	line_vertices.push_back(0);
		// 	line_vertices.push_back((int)(cam_pos.z - far));
		// 	line_vertices.push_back(color.x);
		// 	line_vertices.push_back(color.y);
		// 	line_vertices.push_back(color.z);

		// 	line_vertices.push_back((int)x);
		// 	line_vertices.push_back(0);
		// 	line_vertices.push_back((int)(cam_pos.z + far));
		// 	line_vertices.push_back(color.x);
		// 	line_vertices.push_back(color.y);
		// 	line_vertices.push_back(color.z);
		// }
		// for(float z = cam_pos.z - far; z < cam_pos.z + far; z += 0.5f) {
		// 	glm::vec3 color = {0.4, 0.4, 0.4};
		// 	if((int)z == 0) {
		// 		color = {0.55, 0.8, 0.9};
		// 	}
			
		// 	line_vertices.push_back((int)(cam_pos.x - far));
		// 	line_vertices.push_back(0);
		// 	line_vertices.push_back((int)z);
		// 	line_vertices.push_back(color.x);
		// 	line_vertices.push_back(color.y);
		// 	line_vertices.push_back(color.z);

		// 	line_vertices.push_back((int)(cam_pos.x + far));
		// 	line_vertices.push_back(0);
		// 	line_vertices.push_back((int)z);
		// 	line_vertices.push_back(color.x);
		// 	line_vertices.push_back(color.y);
		// 	line_vertices.push_back(color.z);
		// }
		
		// VertexArray line_va;
		// line_va.create();
		// line_va.bind();

		// size_t vertices_size = line_vertices.size() * sizeof(float);
		// std::vector<VertexAttribute> attribs = {
		// 	{sizeof(float), 3, GL_FALSE},
		// 	{sizeof(float), 3, GL_FALSE}
		// };
		
		// VertexBuffer line_vb;
		// line_vb.create(line_vertices.data(), vertices_size);

		// line_va.addVertexBuffer(line_vb, attribs);

		// line_shader.bind();
		// line_va.bind();

		// glm::mat4 modelmtx = glm::mat4(1.0f);
		// line_shader.setMatrix4("projection", camera.getProjectionMatrix());
        // line_shader.setMatrix4("view", camera.getViewMatrix());
		// line_shader.setMatrix4("model", modelmtx);

		// glDrawArrays(GL_LINES, 0, line_vertices.size());

		// line_va.destroy();
		// line_vb.destroy();
		
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)w, (float)h);
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		window.swap();
	}

	SDL_GL_DeleteContext(context);
	window.destroy();
	
	return 0;
}
