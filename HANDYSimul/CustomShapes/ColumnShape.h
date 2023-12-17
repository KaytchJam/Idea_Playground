#pragma once
#include "SFML/Graphics.hpp"

class ColumnShape : public sf::Drawable {
private:
	sf::CircleShape base, top;
	sf::RectangleShape bar;
public:
	ColumnShape(float radius, float height) {
		top = sf::CircleShape(radius);
		top.setPosition({0, 0});
		top.setFillColor(sf::Color(0x808080FF));

		bar = sf::RectangleShape({ radius * 2, height });
		bar.setPosition({ 0, radius });
		bar.setFillColor(sf::Color(0x000000FF));

		base = sf::CircleShape(radius);
		base.setPosition({ 0, height});
		base.setFillColor(sf::Color(0x000000FF));
	}

	~ColumnShape() {

	}

	// setters

	// Extends the height downwards
	void setHeight(const float height) {
		float top_radius = top.getRadius();
		bar.setSize({ top_radius * 2 , height });
		base.setPosition(top.getPosition() + sf::Vector2f(0, height));
	}

	void setRadius(const float radius) {
		top.setRadius(radius);
		bar.setSize({ radius, bar.getSize().y });
		base.setRadius(radius);
	}

	void setPosition(const sf::Vector2f pos) {
		top.setPosition(pos);
		bar.setPosition(pos + sf::Vector2f(0, top.getRadius()));
		base.setPosition(pos + sf::Vector2f(0, bar.getSize().y));
	}

	void setColor(const uint8_t r, uint8_t g, uint8_t b) {
		int rgb[] = { r, g, b, 1};
		int index = 0;

		int max_color_index = 0;
		for (int i = 1; i < 4; i++) {
			bool cond = (rgb[i] > rgb[max_color_index]);
			max_color_index = cond * rgb[i] + !cond * rgb[max_color_index];
		}

		bar.setFillColor(sf::Color(rgb[0], rgb[1], rgb[2], 0xFF));
		base.setFillColor(sf::Color(rgb[0], rgb[1], rgb[2], 0xFF));
		top.setFillColor(sf::Color(rgb[0] / 3, rgb[1] / 3, rgb[2] / 3, 0xFF));
		
	}

	// getters
	float getHeight() const { return bar.getSize().y; }
	float getRadius() const { return base.getRadius();  }
	sf::Vector2f getPosition() const { return top.getPosition(); }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(base);
		target.draw(bar);
		target.draw(top);
	}
};
