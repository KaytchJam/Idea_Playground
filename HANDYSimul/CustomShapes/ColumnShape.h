#pragma once
#include "SFML/Graphics.hpp"

class ColumnShape : public sf::Drawable {
public:
	enum ColumnType {
		PILLAR,
		BAR
	};
private:
	sf::CircleShape* base;
	sf::CircleShape* top;
	sf::RectangleShape* bar;
	ColumnType mode;
	float m_radius;
	
public:

	ColumnShape(float radius = 0, float height = 0, ColumnType colType = ColumnType::PILLAR) : mode(colType), m_radius(radius) {

		float circle_rad = radius * !(mode == BAR);
		this->top = new sf::CircleShape(circle_rad);
		this->top->setPosition({ 0, 0 });
		this->top->setFillColor(sf::Color(0x808080FF));

		this->bar = new sf::RectangleShape({ radius * 2, height });
		this->bar->setPosition({ 0, radius });
		this->bar->setFillColor(sf::Color(0x000000FF));

		this->base = new sf::CircleShape(circle_rad);
		this->base->setPosition({ 0, height });
		this->base->setFillColor(sf::Color(0x000000FF));
	}

	~ColumnShape() {
		delete top;
		delete base;
		delete bar;
	}

	// setters

	// Extends the height downwards
	ColumnShape& setHeight(const float height) {
		this->bar->setSize({ m_radius * 2 , height });
		this->base->setPosition(top->getPosition() + sf::Vector2f(0, height));
		return *this;
	}

	// set the radius of the column
	ColumnShape& setRadius(const float radius) {
		this->m_radius = radius;
		float circle_rad = m_radius * !(mode == BAR);

		this->top->setRadius(circle_rad);
		this->bar->setSize({ radius, bar->getSize().y });
		this->base->setRadius(circle_rad);
		return *this;
	}

	// set the position of the column
	ColumnShape& setPosition(const sf::Vector2f pos) {
		this->top->setPosition(pos);
		this->bar->setPosition(pos + sf::Vector2f(0, m_radius));
		this->base->setPosition(pos + sf::Vector2f(0, bar->getSize().y));
		return *this;
	}

	// set the color of the bare and base of the column, passing in a byte for each channel
	ColumnShape& setColor(const uint8_t r, uint8_t g, uint8_t b) {
		this->bar->setFillColor(sf::Color(r, g, b, 0xFF));
		this->base->setFillColor(sf::Color(r, g, b, 0xFF));
		this->top->setFillColor(sf::Color(r / 3, g / 3, b / 3, 0xFF));
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
		this->top->setOutlineThickness(thickness);
		//base.setOutlineThickness(thickness);
		return *this;
	}

	ColumnShape& set_outline_color(sf::Color color) {
		this->bar->setOutlineColor(color);
		this->base->setOutlineColor(color);
		this->top->setOutlineColor(color);
		return *this;
	}

	ColumnShape& set_column_type(ColumnType col_type) {
		this->mode = col_type;

		float circle_rad = m_radius * !(mode == BAR);
		this->top->setRadius(circle_rad);
		this->base->setRadius(circle_rad);
		return *this;
	}

	ColumnType get_column_type() {
		return this->mode;
	}

	// get the height of the column
	float getHeight() const { return bar->getSize().y; }
	// get the radius of the column
	float getRadius() const { return m_radius;  }
	// get the position vector of the column
	sf::Vector2f getPosition() const { return top->getPosition(); }
	// get the color of the base and bar of the column
	sf::Color getColor() const { return base->getFillColor(); }

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(*base, states);
		target.draw(*bar, states);
		target.draw(*top, states);
	}
};