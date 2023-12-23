#pragma once
#include "SFML/Graphics.hpp"

class SubCanvas : public sf::Drawable {
private:
	sf::Vector2f origin;
	int LENGTH;
	int HEIGHT;
	std::vector<sf::Drawable*> drawList;
	sf::Transform t;
public:
	SubCanvas();
	~SubCanvas();

	sf::Vector2f getDimensions() const { return { LENGTH, HEIGHT }; }
	sf::Vector2f getOrigin() const { return origin;  }
};