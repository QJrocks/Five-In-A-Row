#include "input.h"

namespace input {
	int cursorRepeatTimer = 0;

	bool piecePlaceHeld = false;
	bool resetBoardHeld = false;

	uint8_t inputFlags;

	void input::updateInputs() {
		inputFlags = 0;

		int LRinput = 0;
		int UDinput = 0;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			UDinput -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			UDinput += 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			LRinput -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			LRinput += 1;
		}


		if (UDinput != 0 || LRinput != 0) {
			if (cursorRepeatTimer == 0 || cursorRepeatTimer >= 20) {
				if (UDinput == -1) {
					inputFlags |= INPUT_UP;
				}
				if (UDinput == 1) {
					inputFlags |= INPUT_DOWN;
				}
				if (LRinput == -1) {
					inputFlags |= INPUT_LEFT;
				}
				if (LRinput == 1) {
					inputFlags |= INPUT_RIGHT;
				}
			}

			if (cursorRepeatTimer >= 20) {
				cursorRepeatTimer -= 3;
			}
			else {
				cursorRepeatTimer++;
			}
		}
		else {
			cursorRepeatTimer = 0;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
			if (!piecePlaceHeld) {
				inputFlags |= INPUT_PLACE;
			}
			piecePlaceHeld = true;
		}
		else {
			piecePlaceHeld = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
			if (!resetBoardHeld) {
				inputFlags |= INPUT_RESET;
			}
			resetBoardHeld = true;
		}
		else {
			resetBoardHeld = false;
		}
	}

	uint8_t getInputFlags() {
		return inputFlags;
	}
}

