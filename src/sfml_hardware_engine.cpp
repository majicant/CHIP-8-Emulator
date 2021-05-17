#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "sfml_hardware_engine.h"

const std::array<sf::Keyboard::Key, 16> SFMLHardwareEngine::key_codes =
{
	sf::Keyboard::Num0, sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3, sf::Keyboard::Num4,
	sf::Keyboard::Num5, sf::Keyboard::Num6, sf::Keyboard::Num7, sf::Keyboard::Num8, sf::Keyboard::Num9,
	sf::Keyboard::A, sf::Keyboard::B, sf::Keyboard::C,
	sf::Keyboard::D, sf::Keyboard::E, sf::Keyboard::F
};

SFMLHardwareEngine::SFMLHardwareEngine(int scale)
	: HardwareEngine(scale)
{
	window.create(sf::VideoMode(64 * display_scale, 32 * display_scale), "Chip-8 Emulator");
}

SFMLHardwareEngine::~SFMLHardwareEngine()
{
	window.close();
}

void SFMLHardwareEngine::DrawPixelInternal(int x, int y, bool pixel_state)
{
	sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(display_scale - 2), static_cast<float>(display_scale - 2)));
	rectangle.setFillColor(pixel_state ? sf::Color::White : sf::Color::Black);
	rectangle.setPosition(static_cast<float>(x * display_scale + 1), static_cast<float>(y * display_scale + 1));
	window.draw(rectangle);
}

void SFMLHardwareEngine::UpdateScreenInternal()
{
	window.display();
}

bool SFMLHardwareEngine::CheckKeyPress(int key_value)
{
	return sf::Keyboard::isKeyPressed(key_codes[key_value]);
}

int SFMLHardwareEngine::WaitForKeyPress()
{
	sf::Event e;
	while (window.pollEvent(e)) {
		if (e.type == e.KeyPressed) {
			for (int i = 0; i < 16; i++) {
				if (sf::Keyboard::isKeyPressed(key_codes[i]))
					return i;
			}
		}
	}
	return -1;
}

bool SFMLHardwareEngine::CheckForExit()
{
	sf::Event e;
	window.pollEvent(e);
	return e.type == sf::Event::Closed;
}