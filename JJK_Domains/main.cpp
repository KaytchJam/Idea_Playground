#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

#include "domain_types/Domain.h"
#include "globals/UserListener.h"

// USER LISTENER external variables
extern kay::keystates user_keys;
extern kay::mousestates user_mouse;

void printVector2f(sf::Vector2f& v) {
	std::printf("{%f,%f}\n", v.x, v.y);
}

void printVector2u(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void printVector2i(sf::Vector2i& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void direction_toggle(sf::Event& event, bool toggle) {
	switch (event.key.scancode) {
	case sf::Keyboard::Scan::Left:
		user_keys.LEFT_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Right:
		user_keys.RIGHT_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Up:
		user_keys.UP_HELD = toggle;
		break;
	case sf::Keyboard::Scan::Down:
		user_keys.DOWN_HELD = toggle;
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

	int i = 0;
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
	if (!font.loadFromFile("assets/fonts/ShareTechMono-Regular.ttf"))
	{
		// error...
		std::cout << "couldn't load font" << std::endl;
		return 0;
	}

	uint8_t opacity = 0x0;

	// GRID INITIALIZATION
	sf::VertexArray grid = make_grid_lines(window, 20, 20, (float) window.getSize().x, (float) window.getSize().y);

	// DOMAIN INITIALIZATION
	ClosedDomain circ(150.f, sf::Color::Blue, 0.5f);
	circ.setOutlineThickness(10.f);
	circ.setCenterPosition(sf::Vector2f((float) window.getSize().x / 2, (float) window.getSize().y / 2));

	ClosedDomain d2(100.f, sf::Color::Red, 0.5f);
	d2.setOutlineThickness(10.f);
	d2.setCenterPosition(circ.getCenterCoords() + sf::Vector2f(circ.getRadius() + circ.getOutlineThickness() + d2.getOutlineThickness() + d2.getRadius(), 0));

	ClosedDomain d3(50.f, sf::Color::Black, 0.5f);
	d3.setOutlineThickness(10.f);

	sf::Transform entity = sf::Transform::Identity;
	sf::RenderStates camera;

	std::cout << "window dimensions: " << "(" << window.getSize().x << "," << window.getSize().y << ")" << std::endl;
	std::cout << "domain 1: " << circ << std::endl;
	std::cout << "domain 2: " << d2 << std::endl;
	std::cout << "distance: " << circ.distance(d2) << std::endl;
	std::cout << "in range? " << circ.inRange(d2) << std::endl;

	std::vector<ClosedDomain*> domainList(2);
	domainList[0] = &circ;
	domainList[1] = &d2;

	std::vector<sf::Text> domainText(domainList.size());
	int index = 0;
	for (sf::Text& dText : domainText) {
		ClosedDomain*& cur = domainList[index];
		dText.setFont(font);
		dText.setCharacterSize(24);
		dText.setFillColor(cur->getColor());
		dText.setPosition(cur->getCenterCoords() + sf::Vector2f(0, cur->getRadius() + cur->getOutlineThickness() + 10));
		dText.setString("Domain " + std::to_string(index + 1));

		std::cout << "Letter spacing: " << dText.getLetterSpacing() << std::endl;
		std::cout << "Bound Size : (" << dText.getGlobalBounds().getSize().x << "," << dText.getGlobalBounds().getSize().y << ")" << std::endl;

		++index;
	}

	sf::Clock dtClock;

	window.setFramerateLimit(30);
	//window.setKeyRepeatEnabled(false);

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
			if (event.type == sf::Event::MouseButtonPressed) {
				user_mouse.MOUSE_HELD = true;
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				user_mouse.MOUSE_HELD = false;
			}
		}

		user_mouse.position = sf::Mouse::getPosition(window);
		//sf::Vector2i localPosition = sf::Mouse::getPosition(window);
		//printVector2i(localPosition);

		window.clear(sf::Color(0xE1, 0xE1, 0xE1));

		if (user_keys.LEFT_HELD) {
			camera.transform = entity.translate(sf::Vector2f(-5.f, 0.f));
			std::cout << "<" << std::endl;
		}
		if (user_keys.RIGHT_HELD) {
			camera.transform = entity.translate(sf::Vector2f(5.f, 0.f));
			std::cout << ">" << std::endl;
		}
		if (user_keys.DOWN_HELD) {
			camera.transform = entity.translate(sf::Vector2f(0.f, 5.f));
			std::cout << "v" << std::endl;
		}
		if (user_keys.UP_HELD) {
			camera.transform = entity.translate(sf::Vector2f(0.f, -5.f));
			std::cout << "^" << std::endl;
		}

		sf::Time elapsed = dtClock.restart();
		//std::cout << "deltaTime: " << elapsed.asSeconds() << std::endl;
		window.draw(grid);
		// Update domains & render them
		for (int i = 0; i < domainList.size(); i++) {
			ClosedDomain* cur = domainList[i];
			cur->onUpdate(elapsed.asSeconds());
			window.draw(*cur, camera);
			domainText[i].setPosition(cur->getCenterCoords() + sf::Vector2f(-1  * domainText[i].getLocalBounds().getSize().x / 2, cur->getRadius() + cur->getOutlineThickness() + 10));
			window.draw(domainText[i], camera);
		}
		window.display();
	}

	return 0;
}