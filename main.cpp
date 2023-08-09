#include <iostream>
#include <SFML/Window.hpp>
#include "board.h"
#include "input.h"
#include "visualResource.h"

namespace gs {
	enum gameState {
		MENU = 0,
		GAME
	};
}

int main() {    
    sf::RenderWindow m_window;
    m_window.create(sf::VideoMode(800, 800), "Base Project", sf::Style::Default);
    m_window.setFramerateLimit(60);

    sf::Event m_event;

    Board theBoard = Board();

	visualResource menuDisplay = visualResource("Img/menu.png");

    theBoard.prepareViews(m_window.getDefaultView(), sf::View(sf::FloatRect(-8, -8, 256, 256)));
    theBoard.clearBoard(0);

	gs::gameState currentState = gs::MENU;
	int menuCursorSelect = 0;
 
    while (m_window.isOpen()) {
        while (m_window.pollEvent(m_event)) {
            switch (m_event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
            }
        }

		input::updateInputs();
		uint8_t inputFlags = input::getInputFlags();
		switch (currentState) {
		case gs::GAME:
			if (inputFlags & input::INPUT_BACK) {
				currentState = gs::MENU;
				break;
			}

			{
				int inputX = 0;
				int inputY = 0;

				if (inputFlags & input::INPUT_LEFT) {
					inputX--;
				}
				if (inputFlags & input::INPUT_RIGHT) {
					inputX++;
				}
				if (inputFlags & input::INPUT_UP) {
					inputY--;
				}
				if (inputFlags & input::INPUT_DOWN) {
					inputY++;
				}

				theBoard.moveCursor(inputX, inputY);
			}


			if (inputFlags & input::INPUT_PLACE) {
				theBoard.placePiece(BS_P1);
			}

			theBoard.update();
			break;
		case gs::MENU:
		{
			if (inputFlags & input::INPUT_UP) {
				menuCursorSelect--;
				if (menuCursorSelect == -1) {
					menuCursorSelect = 3;
				}
			}
			if (inputFlags & input::INPUT_DOWN) {
				menuCursorSelect++;
				if (menuCursorSelect == 4) {
					menuCursorSelect = 0;
				}
			}
			if (inputFlags & input::INPUT_PLACE) {
				currentState = gs::GAME;
				theBoard.clearBoard(0);
			}
		}
		break;
		}

		m_window.clear();
        theBoard.draw(m_window);
		if (currentState == gs::MENU) {
			menuDisplay.draw(m_window, 200, 200);
		}
        m_window.display();
    }
}
