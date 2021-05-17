#include <iostream>

#include "chip8.h"
#include "sdl_hardware_engine.h"
#include "sfml_hardware_engine.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "You did not provide a ROM." << std::endl;
		return 1;
	}

	Chip8 chip8;
	if (!chip8.LoadROM(argv[1])) {
		std::cerr << "Failed to open ROM." << std::endl;
		return 1;
	}

	SDLHardwareEngine engine(15);
	chip8.Run(&engine);

	return 0;
}