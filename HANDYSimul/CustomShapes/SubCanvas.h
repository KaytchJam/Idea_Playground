#pragma once
#include "SFML/Graphics.hpp"
#include "../MyTools/ObjectGroup.h"

class SubCanvas : public sf::Drawable {
private:
	sf::Vector2f origin;
	sf::RectangleShape background;
	sf::RectangleShape shadow;

	int LENGTH;
	int HEIGHT;

	int WINDOW_LENGTH;
	int WINDOW_HEIGHT;

	std::vector<ObjectGroup<sf::Drawable>*> drawList;
	sf::Transform projection;
	
	bool has_shadow;
public:
	SubCanvas(sf::Vector2i subcanvas_size, sf::Vector2i reference_size)
		: LENGTH(subcanvas_size.x), HEIGHT(subcanvas_size.y), WINDOW_LENGTH(reference_size.x), WINDOW_HEIGHT(reference_size.y) {
		projection = sf::Transform((float) subcanvas_size.x / reference_size.x,  0, 0, 
									0, (float) subcanvas_size.y / reference_size.y, 0, 
									0,									         0, 1);
		background = sf::RectangleShape({ (float) subcanvas_size.x, (float) subcanvas_size.y });

		shadow = sf::RectangleShape(background.getSize());
		shadow.setFillColor(sf::Color(0x00000055));
		has_shadow = false;
	}

	~SubCanvas() {}

	SubCanvas& link_group(ObjectGroup<sf::Drawable>* group) {
		drawList.push_back(group);
		return *this;
	}

	sf::Vector2f getDimensions() const { return { (float) LENGTH, (float) HEIGHT }; }
	sf::Vector2f getOrigin() const { return origin; }

	SubCanvas& setPosition(const sf::Vector2f pos) {
		origin = pos;
		projection = sf::Transform((float) LENGTH / WINDOW_LENGTH,  0, pos.x,
									0, (float) HEIGHT / WINDOW_HEIGHT, pos.y,
									0,                              0,     1);
		background.setPosition(pos);
		shadow.setPosition(pos);
		return *this;
	}

	SubCanvas& pull(sf::Vector2f force) {
		background.setPosition(origin + force);
		projection = sf::Transform((float)LENGTH / WINDOW_LENGTH, 0, origin.x + force.x,
								   0, (float)HEIGHT / WINDOW_HEIGHT, origin.y + force.y,
								   0,                                              0, 1);
		return *this;
	}

	SubCanvas& setBackgroundColor(sf::Color color) {
		background.setFillColor(color);
		return *this;
	}

	SubCanvas& set_shadow(bool has_shadow) {
		this->has_shadow = has_shadow;
		return *this;
	}

	sf::Vector2f getCenter() const {
		return { origin.x + LENGTH / 2, origin.y + HEIGHT / 2 };
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (has_shadow) target.draw(shadow, states);

		target.draw(background, states);
		states.transform = projection;

		for (ObjectGroup<sf::Drawable>* og : drawList) {
			og->map_capture([&states, &target](sf::Drawable* t) { target.draw(*t, states); });
		}
	}
};