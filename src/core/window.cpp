#include "window.h"

#include "log.h"

bool Window::create(const char *title, int x, int y, int width, int height,
                    uint32 flags) {
  window = SDL_CreateWindow(title, x, y, width, height, flags);
  ASSERT(window != nullptr);

  return true;
}

void Window::destroy() { SDL_DestroyWindow(window); }

SDL_GLContext Window::createContext() {
  SDL_GLContext context = SDL_GL_CreateContext(window);
  ASSERT(context != nullptr);

  return context;
}

void Window::makeContextCurrent(const SDL_GLContext context) {
  SDL_GL_MakeCurrent(window, context);
}

void Window::swap() { SDL_GL_SwapWindow(window); }

SDL_Window *Window::getNativeWindow() { return window; }

uint32 Window::getID() { return SDL_GetWindowID(window); }

void Window::getViewport(int *w, int *h) { SDL_GetWindowSize(window, w, h); }
