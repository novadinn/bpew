#include "input.h"

#include <map>

static std::map<SDL_Keycode, bool> pressed_keys;
static std::map<SDL_Keycode, bool> released_keys;
static std::map<SDL_Keycode, bool> held_keys;

static std::map<uint8, bool> pressed_mouse_buttons;
static std::map<uint8, bool> released_mouse_buttons;
static std::map<uint8, bool> held_mouse_buttons;

static int wheel_x;
static int wheel_y;

void Input::begin() {
  pressed_keys.clear();
  released_keys.clear();

  pressed_mouse_buttons.clear();
  released_mouse_buttons.clear();

  wheel_x = 0;
  wheel_y = 0;
}

void Input::keyDownEvent(const SDL_Event &event) {
  pressed_keys[event.key.keysym.sym] = true;
  held_keys[event.key.keysym.sym] = true;
}

void Input::keyUpEvent(const SDL_Event &event) {
  released_keys[event.key.keysym.sym] = true;
  held_keys[event.key.keysym.sym] = false;
}

bool Input::wasKeyPressed(SDL_Keycode key) { return pressed_keys[key]; }

bool Input::wasKeyReleased(SDL_Keycode key) { return released_keys[key]; }

bool Input::wasKeyHeld(SDL_Keycode key) { return held_keys[key]; }

void Input::mouseButtonDownEvent(const SDL_Event &event) {
  pressed_mouse_buttons[event.button.button] = true;
  held_mouse_buttons[event.button.button] = true;
}

void Input::mouseButtonUpEvent(const SDL_Event &event) {
  released_mouse_buttons[event.button.button] = true;
  held_mouse_buttons[event.button.button] = false;
}

bool Input::wasMouseButtonPressed(uint8 button) {
  return pressed_mouse_buttons[button];
}

bool Input::wasMouseButtonReleased(uint8 button) {
  return released_mouse_buttons[button];
}

bool Input::wasMouseButtonHeld(uint8 button) {
  return held_mouse_buttons[button];
}

void Input::wheelEvent(const SDL_Event &event) {
  wheel_x = event.wheel.x;
  wheel_y = event.wheel.y;
}

bool Input::wasWheelMovedUp() { return wheel_y > 0; }

bool Input::wasWheelMovedDown() { return wheel_y < 0; }

bool Input::wasWheelMoved() { return wheel_y != 0; }

void Input::getWheelMovement(int *x, int *y) {
  *x = wheel_x;
  *y = wheel_y;
}

void Input::getMousePosition(int *x, int *y) { SDL_GetMouseState(x, y); }
