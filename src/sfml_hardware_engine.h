#pragma once

#include <SFML/Graphics.hpp>

#include "hardware_engine.h"

class SFMLHardwareEngine : public HardwareEngine
{
public:
	SFMLHardwareEngine(int scale);
	~SFMLHardwareEngine();

	bool CheckKeyPress(int key_value);
	int WaitForKeyPress();
	bool CheckForExit();

protected:
	void DrawPixelInternal(int x, int y, bool pixel_state);
	void UpdateScreenInternal();

private:
	static const std::array<sf::Keyboard::Key, 16> key_codes;

	sf::RenderWindow window;
};