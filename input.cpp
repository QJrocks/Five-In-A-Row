#include "input.h"

namespace input {
	int cursorRepeatTimer = 0;

	bool piecePlaceHeld = false;
	bool resetBoardHeld = false;
}

void input::updateInputs(Board& theBoard) {
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


	if (UDinput || LRinput) {
		if (cursorRepeatTimer == 0) {
			theBoard.moveCursor(LRinput, UDinput);
		}

		if (cursorRepeatTimer >= 20) {
			theBoard.moveCursor(LRinput, UDinput);
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
			theBoard.placePiece(BS_P1);
		}
		piecePlaceHeld = true;
	}
	else {
		piecePlaceHeld = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
		theBoard.placePiece(BS_P2);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
		if (!resetBoardHeld) {
			theBoard.clearBoard(0);
		}
		resetBoardHeld = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
		if (!resetBoardHeld) {
			theBoard.clearBoard(1);
		}
		resetBoardHeld = true;
	}
	else {
		resetBoardHeld = false;
	}
}
