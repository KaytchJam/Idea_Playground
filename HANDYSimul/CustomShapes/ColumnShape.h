#pragma once
#include "SFML/Graphics.hpp"

class ColumnShape : public sf::Drawable {
private:
	sf::CircleShape base, top;
	sf::RectangleShape bar;

public:
	ColumnShape(float radius = 0, float height = 0) {
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

	~ColumnShape() {}

	// setters

	// Extends the height downwards
	ColumnShape& setHeight(const float height) {
		float top_radius = top.getRadius();
		bar.setSize({ top_radius * 2 , height });
		base.setPosition(top.getPosition() + sf::Vector2f(0, height));
		return *this;
	}

	// set the radius of the column
	ColumnShape& setRadius(const float radius) {
		top.setRadius(radius);
		bar.setSize({ radius, bar.getSize().y });
		base.setRadius(radius);
		return *this;
	}

	// set the position of the column
	ColumnShape& setPosition(const sf::Vector2f pos) {
		top.setPosition(pos);
		bar.setPosition(pos + sf::Vector2f(0, top.getRadius()));
		base.setPosition(pos + sf::Vector2f(0, bar.getSize().y));
		return *this;
	}

	// set the color of the bare and base of the column, passing in a byte for each channel
	ColumnShape& setColor(const uint8_t r, uint8_t g, uint8_t b) {
		bar.setFillColor(sf::Color(r, g, b, 0xFF));
		base.setFillColor(sf::Color(r, g, b, 0xFF));
		top.setFillColor(sf::Color(r / 3, g / 3, b / 3, 0xFF));
		return *this;
	}

	// set the color of the bar and base of the column passing in 4 bytes at once. The alpha channel is ignored.
	ColumnShape& setColor(uint32_t color) {
		uint8_t colors[3] = {0x0, 0x0, 0x0};
		color = color >> 8; // get rid of alpha channel
		for (int i = 0; i < 3; i++) {
			colors[2 - i] = color & 0xFF;
			color = color >> 8;
		}

		return this->setColor(colors[0], colors[1], colors[2]);
	}

	ColumnShape& set_outline_thickness(float thickness) {
		//bar.setOutlineThickness(thickness);
		top.setOutlineThickness(thickness);
		//base.setOutlineThickness(thickness);
		return *this;
	}

	ColumnShape& set_outline_color(sf::Color color) {
		bar.setOutlineColor(color);
		base.setOutlineColor(color);
		top.setOutlineColor(color);
		return *this;
	}

	// get the height of the column
	float getHeight() const { return bar.getSize().y; }
	// get the radius of the column
	float getRadius() const { return base.getRadius();  }
	// get the position vector of the column
	sf::Vector2f getPosition() const { return top.getPosition(); }
	// get the color of the base and bar of the column
	sf::Color getColor() const { return base.getFillColor(); }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(base, states);
		target.draw(bar, states);
		target.draw(top, states);
	}
};