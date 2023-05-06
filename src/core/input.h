#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <map>

struct Input {
	static void begin();
	static void keyDownEvent(const SDL_Event& event);
	static void keyUpEvent(const SDL_Event& event);

	static bool wasKeyPressed(SDL_Keycode key);
	static bool wasKeyReleased(SDL_Keycode key);
	static bool wasKeyHeld(SDL_Keycode key);
};

#endif // INPUT_H
