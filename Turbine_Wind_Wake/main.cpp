#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

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

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "SORT ANIMATION!");
	sf::CircleShape shape(300.f);
	shape.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));
	/*sf::Texture text;
	sf::Sprite turbine;*/

	/*if (!text.loadFromFile("assets/sprites/Wind_Turbine.png")) {
		std::cout << "failed to load." << std::endl;
	}*/

	/*turbine.setTexture(text);
	turbine.scale(sf::Vector2f(5.f, 5.f));*/

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

		window.clear();

		if (LEFT_HELD) {
			shape.move(sf::Vector2f(-.1f, 0.f));
		}
		if (RIGHT_HELD) {
			shape.move(sf::Vector2f(.1f, 0.f));
		}
		if (DOWN_HELD) {
			shape.move(sf::Vector2f(0.f, .1f));
		}
		if (UP_HELD) {
			shape.move(sf::Vector2f(0.f, -.1f));
		}

		window.draw(shape);
		window.display();
	}

	return 0;
}