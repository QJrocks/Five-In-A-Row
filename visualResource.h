#pragma once
#include <SFML/Graphics.hpp>

//this will have to be re-written
class visualResource {
private:
	sf::Sprite spr;
	sf::Texture txt;

public:
	visualResource();
	visualResource(std::string texturePath);

	sf::Sprite& getSprite();

	void draw(sf::RenderTarget& canvas, float posX, float posY);
};