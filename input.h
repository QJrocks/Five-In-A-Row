#pragma once
#include "board.h"

namespace input {
	void updateInputs(); //call every frame

	uint8_t getInputFlags();

	enum inputFlags {
		INPUT_UP = 0x1 << 0,
		INPUT_DOWN = 0x1 << 1,
		INPUT_LEFT = 0x1 << 2,
		INPUT_RIGHT = 0x1 << 3,
		INPUT_PLACE = 0x1 << 4,
		INPUT_BACK = 0x1 << 5,
		INPUT_RESET = 0x1 << 6,
		INPUT_8 = 0x1 << 7,
	};

}