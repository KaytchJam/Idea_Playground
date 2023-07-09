#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

#include "domain_types/Domain.h"

bool LEFT_HELD = false;
bool RIGHT_HELD = false;
bool UP_HELD = false;
bool DOWN_HELD = false;

void printVector2f(sf::Vector2f& v) {
	std::printf("{%f,%f}\n", v.x, v.y);
}

void printVector2u(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void printVector2i(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void direction_toggle(sf::Event& event, bool toggle) {
	switch (event.key.scancode) {
	case sf::Keyboard::Scan::Left:
		LEFT_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Right:
		RIGHT_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Up:
		UP_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Down:
		DOWN_HELD = toggle;
	}
}

sf::Vector2f getCircleCenter(sf::CircleShape c) {
	sf::Vector2f pt = c.getPoint(0);
	std::cout << pt.x << " " << pt.y << std::endl;
	return sf::Vector2f(c.getPoint(0).x, c.getPoint(0).y + c.getRadius());
}

sf::Vector2f getCircleCenter2(sf::CircleShape c) {
	sf::Vector2f pos = c.getPosition();
	return pos + sf::Vector2f(c.getRadius(), c.getRadius());
}

bool withinRange(float target, float error, float value) {
	return std::abs(value - target) <= error;
}

bool coordsOnCircle(sf::CircleShape c, sf::Vector2i coord) {
	sf::Vector2f center = getCircleCenter2(c);
	float radius = (std::sqrt(std::powf(coord.x - center.x, 2) + std::powf(coord.y - center.y, 2))) + c.getOutlineThickness() / 2;
	/*std::cout << "calced: " << radius << std::endl;
	std::cout << "actual: " << c.getRadius() + c.getOutlineThickness() << std::endl;*/
	return withinRange(c.getRadius() + c.getOutlineThickness() / 2, c.getOutlineThickness() * 2, radius);
}

sf::VertexArray make_grid_lines(sf::RenderWindow& w, int rows, int cols, float length, float height) {
	float x = 0;
	float y = 0;
	float x_shift = length / cols;
	float y_shift = height / rows;

	unsigned int i = 0;
	sf::VertexArray gridlines(sf::Lines, (rows - 1) * (cols - 1) * 2);
	while (i < rows - 1 || i < cols - 1) {
		if (i < rows - 1) {
			float localized_yshift = y + y_shift * (i + 1);
			int actual_index = i << 1; // * 2
			gridlines[actual_index].position = sf::Vector2f(x, localized_yshift);
			gridlines[actual_index + 1].position = sf::Vector2f(x + length, localized_yshift);
		}

		if (i < cols - 1) {
			float localized_xshift = x + x_shift * (i + 1);
			int actual_index = (rows - 1 + i) << 1; // * 2
			gridlines[actual_index].position = sf::Vector2f(localized_xshift, y);
			gridlines[actual_index + 1].position = sf::Vector2f(localized_xshift, y + height);
		}
		i++;
	}

	return gridlines;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Domains.proj");
	///sf::CircleShape shape(300.f);
	//sf::RectangleShape shape(sf::Vector2f(200.f, 200.f));
	//shape.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));

	sf::Vector2i dim(5, 10);

	sf::Font font;
	if (!font.loadFromFile("assets/fonts/arial.ttf"))
	{
		// error...
		std::cout << "couldn't load font" << std::endl;
		return 0;
	}

	uint8_t opacity = 0x0;

	/*sf::Text text;
	text.setFont(font);
	text.setString("Simple Domain");
	text.setCharacterSize(36);
	text.setFillColor(sf::Color(0xFF, 0xFF, 0xFF, opacity));*/
	
	/*sf::VertexArray shape = make_line_strip(window, 800, 600);
	sf::VertexArray lines = make_grid_lines(window, dim.x, dim.y, 800, 600);*/

	// GRID INITIALIZATION
	sf::VertexArray grid = make_grid_lines(window, 20, 20, window.getSize().x, window.getSize().y);

	// DOMAIN INITIALIZATION
	ClosedDomain circ(150.f, sf::Color::Blue, 0.5f);
	circ.setOutlineThickness(10.f);
	circ.setCenterPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));

	ClosedDomain d2(100.f, sf::Color::Red, 0.5f);
	d2.setOutlineThickness(10.f);
	d2.setCenterPosition(circ.getCenterCoords() + sf::Vector2f(circ.getRadius() + circ.getOutlineThickness() + d2.getOutlineThickness() + d2.getRadius(), 0));

	//circ.circle.move(sf::Vector2f(10.f, 10.f));
	///circ.circle.move(sf::Vector2f(window.getSize().x / 2 - circ.getRadius() - circ.circle.getOutlineThickness(), window.getSize().y / 2 - circ.getRadius() - circ.circle.getOutlineThickness()));

	//circ.scale(sf::Vector2f(5.f, 5.f));
	sf::Vector2f center = getCircleCenter2(circ.circle);
	std::printf("Origin: (%f, %f)\n", circ.circle.getOrigin().x, circ.circle.getOrigin().y);
	std::printf("Pos: (%f, %f)\n", circ.circle.getPosition().x, circ.circle.getPosition().y);
	std::printf("circle center: (%f,%f)\n", center.x, center.y);

	sf::Transform entity = sf::Transform::Identity;
	sf::RenderStates states;
	/*sf::Texture text;
	sf::Sprite turbine;*/

	/*if (!text.loadFromFile("assets/sprites/Wind_Turbine.png")) {
		std::cout << "failed to load." << std::endl;
	}*/

	/*turbine.setTexture(text);
	turbine.scale(sf::Vector2f(5.f, 5.f));*/

	//text.move(sf::Vector2f(window.getSize().x / 2 - circ.getRadius() - circ.getOutlineThickness(), window.getSize().y / 2 - circ.getRadius() - circ.getOutlineThickness()));

	std::cout << "window dimensions: " << "(" << window.getSize().x << "," << window.getSize().y << ")" << std::endl;
	std::cout << "domain 1: " << circ << std::endl;
	std::cout << "domain 2: " << d2 << std::endl;
	std::cout << "distance: " << circ.distance(d2) << std::endl;
	std::cout << "in range? " << circ.inRange(d2) << std::endl;

	std::vector<ClosedDomain*> domainList(2);
	domainList[0] = &circ;
	domainList[1] = &d2;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				direction_toggle(event, true);
			}
			if (event.type == sf::Event::KeyReleased) {
				direction_toggle(event, false);
			}
		}

		sf::Vector2i localPosition = sf::Mouse::getPosition(window);

		window.clear(sf::Color(0xE1, 0xE1, 0xE1));

		if (LEFT_HELD) {
			states.transform = entity.translate(sf::Vector2f(-.5f, 0.f));
		}
		if (RIGHT_HELD) {
			states.transform = entity.translate(sf::Vector2f(.5f, 0.f));
		}
		if (DOWN_HELD) {
			states.transform = entity.translate(sf::Vector2f(0.f, .5f));
		}
		if (UP_HELD) {
			states.transform = entity.translate(sf::Vector2f(0.f, -.5f));
		}


		// fade in fade out effect
		/*if (coordsOnCircle(circ.circle, localPosition)) opacity = opacity < 0xFF ? opacity + 1 : 0xFF;
		else opacity = opacity > 0 ? opacity - 1 : 0;

		if (opacity > 0xFF) opacity = 0xFF;
		else if (opacity < 0x0) opacity = 0x0;
		text.setFillColor(sf::Color(0xFF, 0xFF, 0xFF, opacity));*/

		/*window.draw(shape, states);
		window.draw(lines, states);*/

		window.draw(grid);
		// Update domains & render them
		for (int i = 0; i < domainList.size(); i++) {
			domainList[i]->onUpdate(0);
			window.draw(*domainList[i]);
		}
		window.display();
	}

	return 0;
}