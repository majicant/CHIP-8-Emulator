#pragma once

#include <array>
#include <SDL.h>

#include "hardware_engine.h"

class SDLHardwareEngine : public HardwareEngine
{
public:
	SDLHardwareEngine(int scale);
	~SDLHardwareEngine();

	bool CheckKeyPress(int key_value);
	int WaitForKeyPress();
	bool CheckForExit();

protected:
	void DrawPixelInternal(int x, int y, bool pixel_state);
	void UpdateScreenInternal();

private:
	static const std::array<SDL_Scancode, 16> scan_codes;

	SDL_Window* window;
	SDL_Renderer* renderer;
};