#pragma once
#include "UIElement.h"

class UISlider : public UIElement {
private:
	sf::RectangleShape block;
	sf::RectangleShape bar;
	sf::CircleShape pulley;

	float max, min;
	void** m_args;
	const unsigned int m_NUM_ARGS;

public:
	UISlider(float min, float max);
	~UISlider();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};