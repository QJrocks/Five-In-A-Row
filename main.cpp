#include <iostream>
#include <SFML/Window.hpp>
#include "board.h"
#include "input.h"

int main() {    
    sf::RenderWindow m_window;
    m_window.create(sf::VideoMode(800, 800), "Base Project", sf::Style::Default);
    m_window.setFramerateLimit(60);

    sf::Event m_event;

    Board theBoard = Board();

    theBoard.prepareViews(m_window.getDefaultView(), sf::View(sf::FloatRect(-8, -8, 256, 256)));
    theBoard.clearBoard(0);

    while (m_window.isOpen()) {
        while (m_window.pollEvent(m_event)) {
            switch (m_event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;
            }
        }

        input::updateInputs(theBoard);

        theBoard.update();

        m_window.clear();

        theBoard.draw(m_window);

        m_window.display();
    }
}
