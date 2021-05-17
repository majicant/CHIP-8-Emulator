#include <algorithm>

#include "hardware_engine.h"

HardwareEngine::HardwareEngine(int scale)
	: display_scale(std::max(scale, 4))
{
}

void HardwareEngine::UpdateScreen(const std::array<uint8_t, 64 * 32>& display)
{
	for (int x = 0; x < 64; x++) {
		for (int y = 0; y < 32; y++) {
			int position = x + y * 64;
			DrawPixelInternal(x, y, display[position]);
		}
	}
	UpdateScreenInternal();
}