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

	// debug
	std::string clickAlert = "click!";

public:
	UIButton(sf::Vector2f dim, const void** p_args, const unsigned int p_NUM_ARGS, void (*on_click)(const void**, const unsigned int));

	bool checkOverlap(sf::Vector2f pos);
	bool checkOverlap(sf::Vector2i pos);
	
	// button
	UIButton& setPosition(sf::Vector2f pos);
	UIButton& setString(std::string str);
	UIButton& setButtonColor(sf::Color color);

	// font
	UIButton& setFont(sf::Font font);
	UIButton& setFont(std::string font_path);
	UIButton& setTextColor(sf::Color color);
	UIButton& updateClickalAlert(const std::string alert);

	// getters
	sf::Vector2f getPosition() const { return buttonRect.getPosition(); }

	// rendering
	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};