#pragma once

#include "SFML/Graphics.hpp"

#include "../entity/domain_types/DomainManager.h"
#include "UIElement.h"

class UIGroup {
private:
	sf::RectangleShape body;

	struct DomainData {
		enum DomainType domain_type = DomainType::ENUM_SIZE;
		float radius = 0.f;
		sf::Color color;
		float refinment = 0.f;
		sf::Vector2f position;
	};

	struct EntityData {
		int TYPE;
		sf::Vector2f position;
	};

	// INPUT UI elements executed prior to output elements, feed data into struct beforehand
	std::vector<UIElement*> inputElements; 
	std::vector<UIElement*> outputElements;


public:
	UIGroup(sf::Vector2f position, int num_widgets);
	~UIGroup() {}

	void onUpdate(float deltaTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};