#ifndef WINDOW_H
#define WINDOW_H

#include "platform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct Window {

  bool create(const char *title, int x, int y, int width, int height,
              uint32 flags);
  void destroy();

  SDL_GLContext createContext();
  void makeContextCurrent(const SDL_GLContext context);

  void swap();

  SDL_Window *getNativeWindow();
  uint32 getID();
  void getViewport(int *w, int *h);

private:
  SDL_Window *window;
};

#endif // WINDOW_H
