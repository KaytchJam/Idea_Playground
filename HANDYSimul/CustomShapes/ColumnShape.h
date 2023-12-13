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
