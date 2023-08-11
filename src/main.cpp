// clang-format off
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
#include "graphics/mesh.h"
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
// clang-format on

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define NUM_SAMPLES 16
#define CLEAR_COLOR                                                            \
  glm::vec4 { 0.2, 0.2, 0.2, 1.0 }

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigDockingWithShift = true;

  /* adjust the style */
  ImGuiStyle &style = ImGui::GetStyle();
  style.FrameRounding = 6.0f;
  style.WindowBorderSize = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ChildBorderSize = 0.0f;
  style.PopupBorderSize = 0.0f;
  style.WindowRounding = 6.0f;
  style.ChildRounding = 6.0f;
  style.PopupRounding = 6.0f;
  style.ScrollbarRounding = 6.0f;
  style.GrabRounding = 6.0f;
  style.TabRounding = 6.0f;

  ImVec4 *colors = style.Colors;
  colors[ImGuiCol_WindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.28f, 0.02f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.87f, 0.31f, 0.06f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.33f, 0.05f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.87f, 0.29f, 0.04f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.94f, 0.33f, 0.07f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(0.75f, 0.75f, 0.75f, 0.50f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.29f, 0.04f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.93f, 0.31f, 0.05f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.80f);
  colors[ImGuiCol_TabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.43f, 0.43f, 0.44f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.87f, 0.30f, 0.05f, 1.00f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.81f, 0.27f, 0.04f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.30f, 0.00f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.91f, 0.28f, 0.02f, 1.00f);

  Window *window = new Window();
  if (!window->create("BPew", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                      WINDOW_WIDTH, WINDOW_HEIGHT,
                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)) {
    exit(1);
  }

  SDL_GLContext context = window->createContext();
  if (!context) {
    exit(1);
  }

  SDL_Window *native_window = window->getNativeWindow();
  ImGui_ImplSDL2_InitForOpenGL(native_window, context);
  ImGui_ImplOpenGL3_Init("#version 460");

  ImNodes::CreateContext();

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    LOG_ERROR("Failed to initialize GLAD\n");
    exit(1);
  }

  SDL_GL_SetSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Renderer::init();
  Gizmos::init();
  Renderer::setClearColor(CLEAR_COLOR);

  Editor *editor = new Editor();
  editor->create();

  bool running = true;
  while (running) {
    Input::begin();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
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
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
          uint32 window_id = event.window.windowID;
          if (window->getID() == window_id) {
            running = false;
          }
        }
      } break;
      };
    }

    Time::beginFrame();

    editor->onUpdate();

    window->makeContextCurrent(context);

    int w, h;
    window->getViewport(&w, &h);
    glViewport(0, 0, w, h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    editor->onDraw();

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window->swap();
  }

  window->destroy();

  delete editor;
  delete window;

  Gizmos::destroy();
  Renderer::destroy();

  ImNodes::DestroyContext();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(context);

  return 0;
}
