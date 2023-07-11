#include "time.h"

#include <SDL2/SDL.h>

float Time::delta_time;
float Time::previous_time;

float Time::deltaTime() { return delta_time; }

void Time::beginFrame() {
  float current = (float)SDL_GetTicks() / 1000;
  delta_time = current - previous_time;
  previous_time = current;
}
