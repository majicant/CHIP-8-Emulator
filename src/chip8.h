#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

#include "hardware_engine.h"

class Chip8
{
public:
	Chip8();

	bool LoadROM(const std::filesystem::path& filepath);
	void Run(HardwareEngine* hardware_engine);

private:
	uint16_t LoadInstruction();
	void ExecuteInstruction();
	void UpdateTimers(unsigned timer_count);

	void DisplaySprite(int x, int y, int num_bytes);

private:
	static std::array<uint8_t, 80> hex_sprites;

	std::array<uint8_t, 4096> memory{};

	std::array<uint8_t, 16> V{};

	uint16_t I = 0x0000;

	uint8_t DT = 0x00;
	uint8_t ST = 0x00;

	uint16_t PC = 0x0200;
	uint8_t SP = 0x00;

	std::array<uint16_t, 16> stack{};

	std::array<uint8_t, 64 * 32> display{};

	HardwareEngine* engine = nullptr;
};