#pragma once
#include "SFML/Graphics.hpp"

class UISlider {
private:
	sf::RectangleShape block;
	sf::RectangleShape bar;
	sf::CircleShape pulley;

	float max, min;
	void** m_args;
	const unsigned int m_NUM_ARGS;

public:
	UISlider();
	~UISlider();
};