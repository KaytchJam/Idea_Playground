#pragma once
#include "SFML/Graphics.hpp"

struct BoundingBox {
	sf::Vector2f tl, br;
};

class WorldEntity {
private:
	const size_t entity_id;
	static size_t next_entity_id;
public:
	WorldEntity();
	size_t getID() const { return entity_id; }

	virtual sf::Vector2f getPosition() { return sf::Vector2f(0, 0); }
	virtual bool inRange() { return false;  }
	virtual BoundingBox getBoundingBox() { return { sf::Vector2f(0,0), sf::Vector2f(0,0)}; }
};
