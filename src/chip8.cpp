#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "chip8.h"

std::array<uint8_t, 80> Chip8::hex_sprites = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
	std::memcpy(memory.data(), hex_sprites.data(), 80);
}

bool Chip8::LoadROM(const std::filesystem::path& filepath)
{
	std::ifstream rom_fs(filepath, std::ifstream::binary);
	if (!rom_fs.is_open()) {
		return false;
	}

	rom_fs.read(reinterpret_cast<char*>(memory.data() + 0x200), 0xFFF - 0x200);
	rom_fs.close();
	return true;
}

void Chip8::Run(HardwareEngine* hardware_engine)
{
	engine = hardware_engine;
	unsigned cycles = 0;
	while (!engine->CheckForExit()) {
		auto start = std::chrono::high_resolution_clock::now();
		ExecuteInstruction();
		cycles++;
		UpdateTimers(cycles);
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		while (duration.count() < 2) {
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
		}
	}
}

uint16_t Chip8::LoadInstruction()
{
	uint16_t opcode = memory[PC] << 8;
	PC++;
	opcode |= memory[PC];
	PC++;
	return opcode;
}

void Chip8::ExecuteInstruction()
{
	uint16_t opcode = LoadInstruction();
	switch (opcode & 0xF000) {
	case 0x0000:
		switch (opcode & 0x000F) {
		case 0x0000:
			std::memset(display.data(), 0, display.size());
			engine->UpdateScreen(display);
			break;
		case 0x000E:
			SP -= 1;
			PC = stack[SP];
			break;
		}
		break;
	case 0x1000:
		PC = (opcode & 0x0FFF);
		break;
	case 0x2000:
		stack[SP] = PC;
		SP += 1;
		PC = (opcode & 0x0FFF);
		break;
	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			PC += 2;
		break;
	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			PC += 2;
		break;
	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			PC += 2;
		break;
	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		break;
	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		break;
	case 0x8000:
		switch (opcode & 0x000F) {
		case 0x0000:
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0001:
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0002:
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0003:
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0004:
			V[0xF] = (static_cast<uint16_t>(V[(opcode & 0x0F00) >> 8]) + static_cast<uint16_t>(V[(opcode & 0x00F0) >> 4])) > 0xFF;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0005:
			V[0xF] = V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4];
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			break;
		case 0x0006:
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x01;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			break;
		case 0x0007:
			V[0xF] = V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8];
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			break;
		case 0x000E:
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			break;
		}
		break;
	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			PC += 2;
		break;
	case 0xA000:
		I = (opcode & 0x0FFF);
		break;
	case 0xB000:
		PC = (opcode & 0x0FFF) + V[0x0];
		break;
	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = std::rand() & (opcode & 0x00FF);
		break;
	case 0xD000:
		DisplaySprite(V[(opcode & 0x0F00) >> 8], V[(opcode & 0x00F0) >> 4], opcode & 0x000F);
		engine->UpdateScreen(display);
		break;
	case 0xE000:
		switch (opcode & 0x00F0) {
		case 0x0090:
			if (engine->CheckKeyPress(V[(opcode & 0x0F00) >> 8])) {
				PC += 2;
			}
			break;
		case 0x00A0:
			if (!engine->CheckKeyPress(V[(opcode & 0x0F00) >> 8])) {
				PC += 2;
			}
			break;
		}
		break;
	case 0xF000:
		switch (opcode & 0x000F) {
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = DT;
			break;
		case 0x000A:
			V[(opcode & 0x0F00) >> 8] = engine->WaitForKeyPress();
			break;
		case 0x0005:
			switch (opcode & 0x00F0) {
			case 0x0010:
				DT = V[(opcode & 0x0F00) >> 8];
				break;
			case 0x0050:
				std::memcpy(memory.data() + I, V.data(), ((opcode & 0x0F00) >> 8) + 1);
				break;
			case 0x0060:
				std::memcpy(V.data(), memory.data() + I, ((opcode & 0x0F00) >> 8) + 1);
				break;
			}
			break;
		case 0x0008:
			ST = V[(opcode & 0x0F00) >> 8];
			break;
		case 0x000E:
			I += V[(opcode & 0x0F00) >> 8];
			break;
		case 0x0009:
			I = V[(opcode & 0x0F00) >> 8] * 5;
			break;
		case 0x0003:
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] % 100) / 10;
			memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			break;
		}
		break;
	default:
		std::cerr << "Unknown Instruction!" << std::endl;
		break;
	}
}

void Chip8::UpdateTimers(unsigned timer_count)
{
	if (!(timer_count % 8)) {
		if (DT > 0)
			DT -= 1;
		if (ST > 0)
			ST -= 1;
	}
}

void Chip8::DisplaySprite(int x, int y, int num_bytes)
{
	int position;
	int starting_xpos = x;
	V[0xF] = 0;
	for (int i = 0; i < num_bytes; i++) {
		uint8_t byte = (memory.data() + I)[i];
		for (int j = 0; j < 8; j++) {
			uint8_t new_pixel = (byte >> (7 - j)) & 0x01;
			position = x + y * 64;
			if (new_pixel && display[position])
				V[0xF] = 1;
			display[position] ^= new_pixel;
			x = (x + 1) % 64;
		}
		x = starting_xpos;
		y = (y + 1) % 32;
	}
}