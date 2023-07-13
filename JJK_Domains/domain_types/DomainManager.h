#pragma once
#include <vector>
#include <iostream>

#include "Domain.h"
#include "SFML/Graphics.hpp"

class DomainManager : public sf::Drawable {
private:
	std::vector<Domain*> domainList;
public:
	// INSTANTIATION
	DomainManager();
	~DomainManager();

	// INSERTION
	bool add(Domain& d);
	bool add(DomainType type, float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));

	// REMOVAL
	void remove(int index);

	// GETTERS
	size_t size() { return domainList.size(); }
	Domain* get(unsigned int index) const { return domainList[index]; }

	// RENDERING
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void overlapSearch(int index);
};