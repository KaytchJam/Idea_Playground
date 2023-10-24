#include <iostream>

#include "UIButton.h"
#include "../globals\UserListener.h"

extern kay::mousestates user_mouse;

static const std::string DEFAULT_FONT_PATH = "assets/fonts/arial.ttf";

sf::Vector2f centerText(sf::RectangleShape& r, sf::Text t) {
	sf::FloatRect bb = t.getLocalBounds();
	std::printf("box dimensions (%f,%f)\n", bb.getSize().x, bb.getSize().y);
	return r.getPosition() + sf::Vector2f(0.f, r.getSize().y / 2 - bb.getSize().y);
}

//UIButton::UIButton(DomainManager& dm, ButtonType bt, sf::Vector2f dim, sf::Font& font, std::string buttonString) : buttonRect(sf::RectangleShape(dim)), dm(dm) {
//	buttonRect.setFillColor(sf::Color::Black);
//
//	buttonText.setFont(font);
//	buttonText.setCharacterSize(30);
//	buttonText.setFillColor(sf::Color::White);
//	buttonText.setString(buttonString);
//	buttonText.setPosition(centerText(buttonRect, buttonText));
//}

UIButton::UIButton(sf::Vector2f dim, const void** p_args, unsigned int p_NUM_ARGS, void (*on_click)(const void**, const unsigned int)) 
	: UIElement(p_args, p_NUM_ARGS), buttonFunction(on_click) {

	buttonRect = sf::RectangleShape(dim);
	buttonRect.setFillColor(sf::Color::Black);

	if (!buttonFont.loadFromFile(DEFAULT_FONT_PATH)) {
		throw std::invalid_argument("Couldn't properly load default font.");
	}

	buttonText.setFont(buttonFont);
	buttonText.setCharacterSize(30);
	buttonText.setFillColor(sf::Color::White);
	buttonText.setString("DEFAULT");
	buttonText.setPosition(centerText(buttonRect, buttonText));
}

//UIButton::UIButton(DomainManager& dm, ButtonType bt, sf::Vector2f dim, sf::Font& font, std::string buttonString, sf::Vector2f pos) : buttonRect(sf::RectangleShape(dim)), dm(dm) {
//	buttonRect.setPosition(pos);
//	buttonRect.setFillColor(sf::Color::Black);
//
//	buttonText.setFont(font);
//	buttonText.setCharacterSize(30);
//	buttonText.setFillColor(sf::Color::White);
//	buttonText.setString(buttonString);
//	buttonText.setPosition(centerText(buttonRect, buttonText));
//}

void UIButton::setPosition(sf::Vector2f pos) {
	buttonRect.setPosition(pos);
	buttonText.setPosition(centerText(buttonRect, buttonText));
}

void UIButton::setString(std::string str) {
	buttonText.setString(str);
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

bool UIButton::checkOverlap(sf::Vector2i pos) {
	return checkOverlap(sf::Vector2f((float) pos.x, (float) pos.y));
}

void UIButton::setFont(std::string font_path) {
	if (!buttonFont.loadFromFile(DEFAULT_FONT_PATH)) {
		throw std::invalid_argument("Couldn't properly load default font.");
	}

	buttonText.setFont(buttonFont);
}

void UIButton::setFont(sf::Font font) {
	buttonFont = font;
	buttonText.setFont(buttonFont);
}

void UIButton::setTextColor(sf::Color color) {
	buttonText.setFillColor(color);
}

void UIButton::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(buttonRect, states);
	target.draw(buttonText, states);
}

void UIButton::onUpdate(float deltaTime) {
	sf::Vector2f mouse_pos = sf::Vector2f((float) user_mouse.position.x, (float) user_mouse.position.y);
	if (user_mouse.HOLDING_OBJECT == false && user_mouse.LEFT_RELEASED) {
		if (checkOverlap(user_mouse.position) && checkOverlap(user_mouse.LEFT_CLICK_POSITION)) {
			std::cout << "click" << std::endl;
			buttonFunction(m_args, m_NUM_ARGS);
		}
	}
}