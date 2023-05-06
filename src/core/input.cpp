#include "input.h"

#include <map>

static std::map<SDL_Keycode, bool> pressed_keys;
static std::map<SDL_Keycode, bool> released_keys;
static std::map<SDL_Keycode, bool> held_keys;

void Input::begin() {
	pressed_keys.clear();
	released_keys.clear();
}

void Input::keyDownEvent(const SDL_Event& event) {
	pressed_keys[event.key.keysym.sym] = true;
	held_keys[event.key.keysym.sym] = true;
}

void Input::keyUpEvent(const SDL_Event& event) {
	released_keys[event.key.keysym.sym] = true;
	held_keys[event.key.keysym.sym] = false;
}

bool Input::wasKeyPressed(SDL_Keycode key) {
	return pressed_keys[key];
}

bool Input::wasKeyReleased(SDL_Keycode key) {
	return released_keys[key];
}

bool Input::wasKeyHeld(SDL_Keycode key) {
	return held_keys[key];
}
