#pragma once
#include "SFML/Graphics.hpp"
#include <functional>

#include "../entity/domain_types/DomainManager.h"

enum ButtonType {
	BUTTON_ADD,
	BUTTON_REMOVE,
	BUTTON_TRAVEL,
	SIZE = 3
};

class UIButton : public sf::Drawable {
private:
	sf::RectangleShape buttonRect;
	sf::Text buttonText;
	bool (*buttonFunction)(DomainManager& dm, Domain*);
	DomainManager& dm;

	// POSSIBLE FUNCTIONS
	void setButtonFunction(ButtonType bt);

public:
	UIButton(DomainManager& dm, ButtonType bt, sf::Vector2f dim, sf::Font& f, std::string buttonString);
	UIButton(DomainManager& dm, ButtonType bt, sf::Vector2f dim, sf::Font& f, std::string buttonString, sf::Vector2f pos);

	bool checkOverlap(sf::Vector2f pos);
	bool checkOverlap(sf::Vector2i pos);
	
	// button
	void setPosition(sf::Vector2f pos);
	void setButtonColor(sf::Color color);

	// font
	void setFont(sf::Font font);
	void setTextColor(sf::Color color);

	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};