#pragma once
#include <vector>
#include <iostream>

#include "Domain.h"

class DomainManager {
private:

	std::vector<ClosedDomain*> domainList;
public:
	// INSTANTIATION
	DomainManager();
	~DomainManager();

	// INSERTION
	bool add(ClosedDomain d);
	bool add(float radius = 150.f, sf::Color color = sf::Color::Red, float refine_val = 0.5f, sf::Vector2f centerCoords = sf::Vector2f(0.f, 0.f));

};