#pragma once
#include "SFML/Graphics.hpp"

class UIButton : public sf::Drawable {
	sf::RectangleShape buttonRect;
	sf::Text buttonText;
public:
	UIButton(sf::Vector2f dim, sf::Font& f, std::string buttonString);
	UIButton(sf::Vector2f dim, sf::Font& f, std::string buttonString, sf::Vector2f pos);

	bool checkOverlap(sf::Vector2f pos);
	
	// button
	void setPosition(sf::Vector2f pos);
	void setButtonColor(sf::Color color);

	// font
	void setFont(sf::Font font);
	void setTextColor(sf::Color color);

	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};