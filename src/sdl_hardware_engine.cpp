#include <array>
#include <SDL.h>

#include "sdl_hardware_engine.h"

const std::array<SDL_Scancode, 16> SDLHardwareEngine::scan_codes =
{
	SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
	SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
	SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F
};

SDLHardwareEngine::SDLHardwareEngine(int scale)
	: HardwareEngine(scale)
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		64 * display_scale, 32 * display_scale, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
}

SDLHardwareEngine::~SDLHardwareEngine()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void SDLHardwareEngine::DrawPixelInternal(int x, int y, bool pixel_state)
{
	Uint8 colour = pixel_state ? 0xFF : 0x00;
	SDL_SetRenderDrawColor(renderer, colour, colour, colour, SDL_ALPHA_OPAQUE);
	SDL_Rect fillRect = { x * display_scale + 1, y * display_scale + 1, display_scale - 2, display_scale - 2 };
	SDL_RenderFillRect(renderer, &fillRect);
}

void SDLHardwareEngine::UpdateScreenInternal()
{
	SDL_RenderPresent(renderer);
}

bool SDLHardwareEngine::CheckKeyPress(int key_value)
{
	const Uint8* keyboard_state = SDL_GetKeyboardState(nullptr);
	SDL_PumpEvents();
	return keyboard_state[scan_codes[key_value]];
}

int SDLHardwareEngine::WaitForKeyPress()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			for (int i = 0; i < 16; i++) {
				if (scan_codes[i] == e.key.keysym.scancode)
					return i;
			}
		}
	}
	return -1;
}

bool SDLHardwareEngine::CheckForExit()
{
	SDL_Event e;
	SDL_PollEvent(&e);
	return e.type == SDL_QUIT;
}