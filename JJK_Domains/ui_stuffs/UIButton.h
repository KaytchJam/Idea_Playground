#pragma once
#include "UIElement.h"
#include <functional>

class UIButton : public UIElement {
private:
	// rendering
	sf::RectangleShape buttonRect;
	sf::Font buttonFont;
	sf::Text buttonText;

	// function
	void (*buttonFunction)(const void**, const unsigned int);

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
	void setFont(std::string font_path);
	void setTextColor(sf::Color color);

	// getters
	sf::Vector2f getPosition() const { return buttonRect.getPosition(); }

	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};