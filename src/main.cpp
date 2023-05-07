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
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

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
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigDockingWithShift = true;
	
	SDL_Window* native_window = window.getNativeWindow();
	ImGui_ImplSDL2_InitForOpenGL(native_window, context);
	ImGui_ImplOpenGL3_Init("#version 460");
	
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		LOG_ERROR("Failed to initialize GLAD\n");
		exit(1);
    }
	
	SDL_GL_SetSwapInterval(1);

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
	
	Renderer::init();
	
	Texture2D texture;
	texture.createFromFile(FileSystem::joinPath("img/wall.jpg").c_str());

	VertexArray va;
	VertexBuffer vb;

	va.create();
	va.bind();

	std::vector<VertexAttribute> attribs = {
		{sizeof(float), 3, GL_FALSE},
	    {sizeof(float), 2, GL_FALSE},
	};

	vb.create(vertices, sizeof(vertices));
	va.addVertexBuffer(vb, attribs);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

	Camera camera;
	camera.create(45, 1.778f, 0.1f, 1000);
	
	glClearColor(0.1, 0.1, 0.1, 1);

	float last_time = SDL_GetTicks();

	glm::ivec2 previous_mouse;
	
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

		glm::ivec2 current_mouse;
		Input::getMousePosition(&current_mouse.x, &current_mouse.y);
		glm::vec2 mouse_delta = current_mouse - previous_mouse;
		mouse_delta *= delta_time;
		previous_mouse = current_mouse;

		glm::ivec2 wheel_movement;
		Input::getWheelMovement(&wheel_movement.x, &wheel_movement.y);
		
		if(Input::wasMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
			if(Input::wasKeyHeld(SDLK_LSHIFT)) {
				camera.pan(mouse_delta);
			} else {
				camera.rotate(mouse_delta);
			}
		}
		if(Input::wasWheelMoved()) {
			camera.zoom(delta_time * wheel_movement.y);
		}
		
		window.makeContextCurrent(context);
		
		int w, h;
		window.getViewport(&w, &h);
		glViewport(0, 0, w, h);

		camera.setViewportSize(w, h);
		
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
		
		Renderer::clear();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		
		texture.bind();
		
        Renderer::begin(camera.getViewMatrix(), camera.getProjectionMatrix());

		glm::mat4 model = glm::mat4(1.0f);
		Renderer::drawMesh(model);
		
		va.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		Renderer::end();

		// From -camera.far to +camera.far
		for(float x = -1000.0f; x < 1000.0f; x += 0.5f) {
			for(float y = -1000.0f; y < 1000.0f; y += 0.5f) {
				
			}
		}
		
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)w, (float)h);
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		window.swap();
	}

	Renderer::destroy();
	
	SDL_GL_DeleteContext(context);
	window.destroy();
	
	return 0;
}
