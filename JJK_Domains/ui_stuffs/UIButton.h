#pragma once
#include "SFML/Graphics.hpp"
#include <functional>

#include "../entity/domain_types/DomainManager.h"

class UIButton : public sf::Drawable {
private:
	// rendering
	sf::RectangleShape buttonRect;
	sf::Text buttonText;

	// function
	void (*buttonFunction)(const void**, const unsigned int);
	const void** m_args;
	const unsigned int m_NUM_ARGS;

public:
	UIButton(sf::Vector2f dim, const void** p_args, const unsigned int p_NUM_ARGS, void (*on_click)(const void**, const unsigned int));

	bool checkOverlap(sf::Vector2f pos);
	bool checkOverlap(sf::Vector2i pos);
	
	// button
	void setPosition(sf::Vector2f pos);
	void setString(std::string str);
	void setButtonColor(sf::Color color);

	// font
	void setFont(sf::Font font);
	void setTextColor(sf::Color color);

	// getters
	sf::Vector2f getPosition() const { return buttonRect.getPosition(); }

	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};