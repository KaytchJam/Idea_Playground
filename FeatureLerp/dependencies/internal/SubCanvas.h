#pragma once
#include "SFML/Graphics.hpp"
#include "PointerVector.h"

class SubCanvas : public sf::Drawable {
private:
	sf::Vector2f origin;
	sf::RectangleShape* background;
	sf::RectangleShape* shadow;

	int LENGTH;
	int HEIGHT;

	int WINDOW_LENGTH;
	int WINDOW_HEIGHT;

	std::vector<PointerVector<sf::Drawable>*> drawList;
	sf::Transform* projection;
	
	bool has_shadow;
public:
	SubCanvas(sf::Vector2i subcanvas_size, sf::Vector2i reference_size)
		: LENGTH(subcanvas_size.x), HEIGHT(subcanvas_size.y), WINDOW_LENGTH(reference_size.x), WINDOW_HEIGHT(reference_size.y) {
		projection = new sf::Transform((float) subcanvas_size.x / reference_size.x,  0, 0, 
									    0, (float) subcanvas_size.y / reference_size.y, 0, 
									    0,									         0, 1);
		background = new sf::RectangleShape({ (float) subcanvas_size.x, (float) subcanvas_size.y });

		shadow = new sf::RectangleShape(background->getSize());
		shadow->setFillColor(sf::Color(0x00000055));
		has_shadow = false;
	}

	~SubCanvas() {
		delete background;
		delete shadow;
		delete projection;
		// the subcanvas doesn't take ownership of the pointers provided to it
	}

	// provide a drawable to be projected onto this canvas
	SubCanvas& link_group(PointerVector<sf::Drawable>* group) {
		drawList.push_back(group);
		return *this;
	}

	sf::Vector2f getDimensions() const { return { (float) LENGTH, (float) HEIGHT }; }
	sf::Vector2f getOrigin() const { return origin; }

	// get the origin of this subcanvas (top_left corner)
	SubCanvas& setPosition(const sf::Vector2f pos) {
		origin = pos;
		projection = new sf::Transform((float) LENGTH / WINDOW_LENGTH,  0, pos.x,
									0, (float) HEIGHT / WINDOW_HEIGHT, pos.y,
									0,                              0,     1);
		background->setPosition(pos);
		shadow->setPosition(pos);
		return *this;
	}

	// move the canvas by some vector shift
	SubCanvas& move(const sf::Vector2f shift) {
		origin += shift;
		projection->translate(shift);
		background->setPosition(background->getPosition() + shift);
		shadow->setPosition(background->getPosition() + shift);
		return *this;
	}

	// displace the canvas from its origin based on some vector force
	SubCanvas& pull(sf::Vector2f force) {
		background->setPosition(origin + force);
		projection = new sf::Transform((float)LENGTH / WINDOW_LENGTH, 0, origin.x + force.x,
								   0, (float)HEIGHT / WINDOW_HEIGHT, origin.y + force.y,
								   0,                                              0, 1);
		return *this;
	}

	// set the background color
	SubCanvas& setBackgroundColor(sf::Color color) {
		background->setFillColor(color);
		return *this;
	}

	// enable a drop-down shadow
	SubCanvas& set_shadow(bool has_shadow) {
		this->has_shadow = has_shadow;
		return *this;
	}

	// set outline thickness to some float value 'thickness'
	SubCanvas& set_outline_thickness(float thickness) {
		this->background->setOutlineThickness(thickness);
		return *this;
	}

	// set the outline color to some sf::Color color
	SubCanvas& set_outline_color(sf::Color color) {
		this->background->setOutlineColor(color);
		return *this;
	}

	// get the center of this canvas (origin.x + LENGTH /2 , origin.y + HEIGHT / 2)
	sf::Vector2f getCenter() const {
		return { origin.x + LENGTH / 2, origin.y + HEIGHT / 2 };
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		if (has_shadow) target.draw(*shadow, states);

		target.draw(*background, states);
		states.transform = *projection;

		for (PointerVector<sf::Drawable>* og : drawList) {
			og->map_capture([&states, &target](sf::Drawable* t) { target.draw(*t, states); });
		}
	}
};