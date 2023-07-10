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

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	sf::CircleShape circle;
	// Construction / Destruction
	ClosedDomain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));
	~ClosedDomain() {}

	// Getters
	sf::Vector2f getCenterCoords() const;
	float getRadius() const { return circle.getRadius(); }
	float getBaseRadius() const { return base_radius; }
	float getTrueRadius() const { return base_radius + circle.getOutlineThickness(); }
	float getOutlineThickness() const { return circle.getOutlineThickness(); }
	sf::Color getColor() const { return line_color; }

	// Setters
	void setColor(sf::Color color) { circle.setOutlineColor(color); }
	void setOutlineThickness(float val) { circle.setOutlineThickness(val); }
	void setOriginPosition(sf::Vector2f pos) { circle.setPosition(pos); base_origin_position = pos; }
	void setCenterPosition(sf::Vector2f pos);

	// Relational
	float distance(ClosedDomain& other);
	bool inRange(ClosedDomain& other);
	void consume(ClosedDomain& other);

	// Render Loop
	void onUpdate(float deltaTime);
};

std::ostream& operator<<(std::ostream& stream, const ClosedDomain& d);