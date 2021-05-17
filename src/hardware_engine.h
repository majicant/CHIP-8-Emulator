#pragma once

#include <array>

class HardwareEngine
{
public:
	HardwareEngine(int scale);
	virtual ~HardwareEngine() = default;

	void UpdateScreen(const std::array<uint8_t, 64 * 32>& display);

	virtual bool CheckKeyPress(int key_value) = 0;
	virtual int WaitForKeyPress() = 0;
	virtual bool CheckForExit() = 0;

protected:
	const int display_scale;

	virtual void DrawPixelInternal(int x, int y, bool pixel_state) = 0;
	virtual void UpdateScreenInternal() = 0;
};