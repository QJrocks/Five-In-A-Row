#include "visualResource.h"

visualResource::visualResource() {
	//Ideally this should never be run
}

visualResource::visualResource(std::string texturePath) {
	txt.loadFromFile(texturePath);
}

sf::Sprite& visualResource::getSprite() {
	return spr;
}

void visualResource::draw(sf::RenderTarget& canvas, float posX, float posY) {
	spr.setPosition(posX, posY);
	spr.setTexture(txt, true);
	canvas.draw(spr);
}
