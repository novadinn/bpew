#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "core/utils.h"
#include "core/window.h"
#include "core/log.h"
#include "core/input.h"
#include "core/time.h"
#include "graphics/shaders/shader.h"
#include "graphics/texture.h"
#include "graphics/index_buffer.h"
#include "graphics/vertex_buffer.h"
#include "graphics/vertex_array.h"
#include "graphics/renderer.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/model.h"
#include "graphics/framebuffer.h"
#include "graphics/gizmos.h"
#include "ecs/scene.h"
#include "ecs/entity.h"
#include "editor/editor.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "ImGuizmo/ImGuizmo.h"
#include "imnodes/imnodes.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define NUM_SAMPLES 16
#define CLEAR_COLOR glm::vec4{0.2, 0.2, 0.2, 1.0}

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
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, NUM_SAMPLES);
	
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
  
  ImNodes::CreateContext();
  
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD\n");
    exit(1);
  }
	
  SDL_GL_SetSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  Renderer::init();
  Gizmos::init();
  Renderer::setClearColor(CLEAR_COLOR);

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

    Time::beginFrame();
		
    editor.onUpdate();
		
    window.makeContextCurrent(context);
		
    int w, h;
    window.getViewport(&w, &h);
    glViewport(0, 0, w, h);
		
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    editor.onDraw();
		
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
    window.swap();
  }

  Gizmos::destroy();
  Renderer::destroy();

  ImNodes::DestroyContext();
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
	
  SDL_GL_DeleteContext(context);
  window.destroy();
	
  return 0;
}
