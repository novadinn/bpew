#ifndef INPUT_H
#define INPUT_H

#include "platform.h"

#include <SDL2/SDL.h>
#include <map>

struct Input {
	static void begin();
	static void keyDownEvent(const SDL_Event& event);
	static void keyUpEvent(const SDL_Event& event);

	static bool wasKeyPressed(SDL_Keycode key);
	static bool wasKeyReleased(SDL_Keycode key);
	static bool wasKeyHeld(SDL_Keycode key);

	static void mouseButtonDownEvent(const SDL_Event& event);
	static void mouseButtonUpEvent(const SDL_Event& event);

	static bool wasMouseButtonPressed(uint8 button);
	static bool wasMouseButtonReleased(uint8 button);
	static bool wasMouseButtonHeld(uint8 button);

	static void wheelEvent(const SDL_Event& event);

	static bool wasWheelMovedUp();
	static bool wasWheelMovedDown();
	static bool wasWheelMoved();
	
	static void getWheelMovement(int* x, int* y);
	
	static void getMousePosition(int* x, int* y);
};

#endif // INPUT_H
