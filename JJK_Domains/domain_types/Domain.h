#pragma once
#include "SFML/Graphics.hpp"


class ClosedDomain : public sf::Drawable {
private:
	// ANIMATION
	float degree = 0;
	const float DEGREES_PER_FRAME = 100.f;
	sf::Color line_color;
	sf::Vector2f base_origin_position = sf::Vector2f(0, 0);

	// PARAMS
	float refinement;
	float base_radius; 

	// STATES
	bool TUG_OF_WAR = false;
	bool IDLE = true;
	bool SELECTED = false;
	bool CONSUMED = false;

public:
	sf::CircleShape circle;
	// Construction / Destruction
	ClosedDomain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));
	~ClosedDomain() {}

	// Getters
	sf::Vector2f getCenterCoords() const;
	sf::Vector2f getOriginCoords() const { return circle.getPosition(); }
	sf::Vector2f getBaseOriginCoords() const { return base_origin_position; }
	sf::Color getColor() const { return line_color; }
	float getRadius() const { return circle.getRadius(); }
	float getBaseRadius() const { return base_radius; }
	float getTrueRadius() const { return base_radius + circle.getOutlineThickness(); }
	float getOutlineThickness() const { return circle.getOutlineThickness(); }
	float getRefinement() const { return refinement; }

	// Setters
	void setColor(sf::Color color) { circle.setOutlineColor(color); }
	void setOutlineThickness(float val) { circle.setOutlineThickness(val); }
	void setOriginPosition(sf::Vector2f pos) { circle.setPosition(pos); base_origin_position = pos; }
	void setCenterPosition(sf::Vector2f pos);

	// Relational
	float distance(ClosedDomain& other);
	bool inRange(ClosedDomain& other);
	void consume(ClosedDomain& other);
	bool isConsumed() const { return CONSUMED;  }
	bool isSelected() const { return SELECTED;  }

	// Render Loop
	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	// Misc + Utility
	static sf::Vector2f centerToOriginCoords(sf::Vector2f pos, float radius) { return  pos - sf::Vector2f(radius, radius); }
};

std::ostream& operator<<(std::ostream& stream, const ClosedDomain& d);
