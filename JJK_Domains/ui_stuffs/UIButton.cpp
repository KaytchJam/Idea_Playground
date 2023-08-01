#include "UIButton.h"

UIButton::UIButton(sf::Vector2f dim) : buttonRect(sf::RectangleShape(dim)) {

}

UIButton::UIButton(sf::Vector2f dim, sf::Vector2f pos) : buttonRect(sf::RectangleShape(dim)) {
	buttonRect.setPosition(pos);
}