#pragma once
#include "SFML/Graphics.hpp"

class UIButton {
	sf::RectangleShape buttonRect;
public:
	UIButton(sf::Vector2f dim);
	UIButton(sf::Vector2f dim, sf::Vector2f pos);

	bool checkOverlap(sf::Vector2f pos);
	
	// button
	void setPosition(sf::Vector2f pos);
	void setButtonColor(sf::Color);

	// font
	void setFont(sf::Text);
	void setFontColor(sf::Color);
};