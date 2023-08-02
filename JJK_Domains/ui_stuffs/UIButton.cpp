#include "UIButton.h"
#include <iostream>

sf::Vector2f centerText(sf::RectangleShape& r, sf::Text t) {
	sf::FloatRect bb = t.getLocalBounds();
	std::printf("box dimensions (%f,%f)\n", bb.getSize().x, bb.getSize().y);
	return r.getPosition() + sf::Vector2f(0.f, r.getSize().y / 2 - t.getLocalBounds().getSize().y / 2);
}

UIButton::UIButton(sf::Vector2f dim, sf::Font& font, std::string buttonString) : buttonRect(sf::RectangleShape(dim)) {
	buttonRect.setFillColor(sf::Color::Black);

	buttonText.setFont(font);
	buttonText.setCharacterSize(30);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setString(buttonString);
	buttonText.setPosition(centerText(buttonRect, buttonText));
}

UIButton::UIButton(sf::Vector2f dim, sf::Font& font, std::string buttonString, sf::Vector2f pos) : buttonRect(sf::RectangleShape(dim)) {
	buttonRect.setPosition(pos);
	buttonRect.setFillColor(sf::Color::Black);

	buttonText.setFont(font);
	buttonText.setCharacterSize(30);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setString(buttonString);
	buttonText.setPosition(centerText(buttonRect, buttonText));
}

void UIButton::setPosition(sf::Vector2f pos) {
	buttonRect.setPosition(pos);
	buttonText.setPosition(centerText(buttonRect, buttonText));
}

void UIButton::setButtonColor(sf::Color color) {
	buttonRect.setFillColor(color);
}

bool UIButton::checkOverlap(sf::Vector2f pos) {
	sf::Vector2f buttonPos = buttonRect.getPosition();
	sf::Vector2f buttonSize = buttonRect.getSize();

	bool xBounds = pos.x > buttonPos.x && pos.x < buttonPos.x + buttonSize.x;
	bool yBounds = pos.y > buttonPos.y && pos.y < buttonPos.y + buttonSize.y;
	return xBounds && yBounds;
}

void UIButton::setFont(sf::Font font) {
	buttonText.setFont(font);
}

void UIButton::setTextColor(sf::Color color) {
	buttonText.setFillColor(color);
}

void UIButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(buttonRect, states);
	target.draw(buttonText, states);
}