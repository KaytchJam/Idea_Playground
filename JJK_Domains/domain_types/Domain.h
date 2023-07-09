#pragma once
#include "SFML/Graphics.hpp"


class ClosedDomain : public sf::Drawable {
private:
	// ANIMATION
	float degree = 0;
	const float DEGREES_PER_FRAME = 1.f;

	// PARAMS
	float refinement;
	float base_radius; 

	// STATES
	bool TUG_OF_WAR = false;
	bool IDLE = true;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	sf::CircleShape circle;
	// Construction / Destruction
	ClosedDomain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));
	~ClosedDomain() {}

	// Getters
	sf::Vector2f getCenterCoords() const;
	float getRadius() const { return base_radius; }
	float getTrueRadius() const { return base_radius + circle.getOutlineThickness(); }
	float getOutlineThickness() const { return circle.getOutlineThickness(); }

	// Setters
	void setColor(sf::Color color) { circle.setOutlineColor(color); }
	void setOutlineThickness(float val) { circle.setOutlineThickness(val); }
	void setPosition(sf::Vector2f pos) { circle.setPosition(pos); }

	// Relational
	float distance(ClosedDomain& other);
	bool inRange(ClosedDomain& other);
	void consume(ClosedDomain& other);

	// Render Loop
	void onUpdate();
};

std::ostream& operator<<(std::ostream& stream, const ClosedDomain& d);