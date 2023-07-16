#pragma once
#include "SFML/Graphics.hpp"

enum DomainType {
	CLOSED_DOMAIN = 0,
	OPEN_DOMAIN = 1,
	SIMPLE_DOMAIN = 2,
	ENUM_SIZE = SIMPLE_DOMAIN + 1
};

class Domain : public sf::Drawable {
protected:
	// ANIMATION
	float degree = 0;
	const float DEGREES_PER_FRAME = 100.f;
	sf::Color line_color;
	sf::Vector2f base_origin_position = sf::Vector2f(0, 0);

	// PARAMS
	float refinement = 0.0f;
	float base_radius = 0.0f;
	DomainType type = DomainType::ENUM_SIZE;

	// STATES
	bool TUG_OF_WAR = false;
	bool IDLE = true;
	bool SELECTED = false;
	bool CONSUMED = false;
	bool SURE_HIT_ACTIVE = true;

public:
	sf::CircleShape circle;
	// Construction / Destruction
	Domain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f originCoords = sf::Vector2f(0.f, 0.f));
	~Domain() {}

	// Getters
	sf::Vector2f getCenterCoords() const;
	sf::Vector2f getOriginCoords() const { return circle.getPosition(); }
	sf::Vector2f getBaseOriginCoords() const { return base_origin_position; }
	sf::Color getColor() const { return line_color; }
	DomainType getDomainType() const { return type;  }

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
	float distance(Domain& other);
	bool inRange(Domain& other);
	virtual void consume(Domain& other) {}
	bool isConsumed() const { return CONSUMED; }
	bool isSelected() const { return SELECTED; }

	// Render Loop
	void onUpdate(float deltaTime);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	// Misc + Utility
	static sf::Vector2f centerToOriginCoords(sf::Vector2f pos, float radius) { return  pos - sf::Vector2f(radius, radius); }
};

// CLOSED DOMAIN
class ClosedDomain : public Domain {
public:
	ClosedDomain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f originCoords = sf::Vector2f(0.f, 0.f));
	~ClosedDomain();

	void consume(Domain& other);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

// OPEN DOMAIN
class OpenDomain : public Domain {
private:
	static sf::Shader* odShader;
public:
	OpenDomain(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f originCoords = sf::Vector2f(0.f, 0.f));
	~OpenDomain() {}

	void consume(Domain& other) {}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

// SIMPLE DOMAIN
class SimpleDomain : public Domain {
	SimpleDomain(float radius = 30.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f originCoords = sf::Vector2f(0.f, 0.f)) {}
	~SimpleDomain() {}

	void consume(Domain& other) {}
	//void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

std::ostream& operator<<(std::ostream& stream, const Domain& d);
