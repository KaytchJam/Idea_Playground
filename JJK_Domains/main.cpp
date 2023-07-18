#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

#include "domain_types/Domain.h"
#include "domain_types/DomainManager.h"
#include "globals/UserListener.h"

// USER LISTENER external variables
extern kay::keystates user_keys;
extern kay::mousestates user_mouse;

//void printVector2f(sf::Vector2f& v) {
//	std::printf("{%f,%f}\n", v.x, v.y);
//}

void printVector2f(sf::Vector2f v) {
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

	sf::Font font;
	if (!font.loadFromFile("assets/fonts/ShareTechMono-Regular.ttf"))
	{
		// error...
		std::cout << "couldn't load font" << std::endl;
		return 0;
	}

	if (!sf::Shader::isAvailable()) {
		std::cout << "shaders not available on device." << std::endl;
		return false;
	}

	uint8_t opacity = 0x0;

	// GRID INITIALIZATION
	sf::VertexArray grid = make_grid_lines(window, 20, 20, (float) window.getSize().x, (float) window.getSize().y);
	// DOMAIN INITIALIZATION
	ClosedDomain d1(100.f, sf::Color::Blue, 1.3f, ClosedDomain::centerToOriginCoords(sf::Vector2f((float)window.getSize().x / 2, (float)window.getSize().y / 2), 150.f));
	ClosedDomain d2(100.f, sf::Color::Red, 1.1f);
	d2.setCenterPosition(d1.getCenterCoords() + sf::Vector2f(d1.getRadius() + d1.getOutlineThickness() + d2.getOutlineThickness() + d2.getRadius() + 20, 0));

	/*ClosedDomain d3(50.f, sf::Color::Black, 0.5f);
	d3.setOutlineThickness(10.f);*/
	
	sf::RectangleShape rect(sf::Vector2f(200.f, 100.f));
	rect.setPosition(50.f, (float) window.getSize().y - 150);
	rect.setFillColor(sf::Color::Black);

	DomainManager dList;
	//dList.add(d1);
	//dList.add(d2);
	dList.add(DomainType::OPEN_DOMAIN, 100.f, sf::Color::Black, 2.f);
	//dList.add(DomainType::OPEN_DOMAIN, 50.f, sf::Color::Green, 1.1f, sf::Vector2f(800.f, 500.f));
	//dList.add(CLOSED_DOMAIN, 100.f, sf::Color::Magenta, 1.f);
	//dList.add(CLOSED_DOMAIN, 100.f, sf::Color::Black, 1.4f, sf::Vector2f(200, 500));

	sf::Transform entity = sf::Transform::Identity;
	sf::RenderStates camera;

	OpenDomain d(50.f, sf::Color::Black, 1.f, sf::Vector2f(300.f, 300.f));

	/*std::cout << "window dimensions: " << "(" << window.getSize().x << "," << window.getSize().y << ")" << std::endl;
	std::cout << "domain 1: " << d1 << std::endl;
	std::cout << "domain 2: " << d2 << std::endl;
	std::cout << "distance: " << d1.distance(d2) << std::endl;
	std::cout << "in range? " << d1.inRange(d2) << std::endl;*/

	std::vector<sf::Text> domainText(dList.size());
	int index = 0;
	for (sf::Text& dText : domainText) {
		Domain* cur = dList.get(index);
		dText.setFont(font);
		dText.setCharacterSize(24);
		dText.setFillColor(cur->getColor());
		dText.setPosition(cur->getCenterCoords() + sf::Vector2f(0, cur->getRadius() + cur->getOutlineThickness() + 10));
		dText.setString("Domain " + std::to_string(index + 1) + "\nRef: " + std::to_string(cur->getRefinement()));

		std::cout << "Letter spacing: " << dText.getLetterSpacing() << std::endl;
		std::cout << "Bound Size : (" << dText.getGlobalBounds().getSize().x << "," << dText.getGlobalBounds().getSize().y << ")" << std::endl;

		++index;
	}

	std::cout << std::endl;

	sf::Clock dtClock;

	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);

	for (int i = 0; i < dList.size(); i++) {
		printVector2f(dList.get(i)->circle.getPosition());
	}

	bool once = false;

	while (window.isOpen()) {
		user_mouse.MOUSE_RELEASED = false;
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed) { direction_toggle(event, true); }
			if (event.type == sf::Event::KeyReleased) { direction_toggle(event, false); }
			if (event.type == sf::Event::MouseButtonPressed) { user_mouse.MOUSE_HELD = true; }
			if (event.type == sf::Event::MouseButtonReleased) {
				user_mouse.MOUSE_HELD = false;
				user_mouse.HOLDING_OBJECT = false;
				user_mouse.MOUSE_RELEASED = true;
			}
		}

		user_mouse.position = sf::Mouse::getPosition(window);
		//sf::Vector2i localPosition = sf::Mouse::getPosition(window);
		//printVector2i(localPosition);

		window.clear(sf::Color(0xE1, 0xE1, 0xE1));

		//std::cout << "Holding Object: " << user_mouse.HOLDING_OBJECT << std::endl;

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

		/*sf::Vector2f rPos = rect.getPosition();
		if (user_mouse.position.x > rPos.x && user_mouse.position.y > rPos.y && user_mouse.position.x < rPos.x + rect.getSize().x && user_mouse.position.y < rPos.y + rect.getSize().y) {
			if (user_mouse.MOUSE_RELEASED) {
				dList.add(DomainType::CLOSED_DOMAIN, 100.f, sf::Color::Black, 1.f);
			}
		}*/

		sf::Time elapsed = dtClock.restart();

		//std::cout << "deltaTime: " << elapsed.asSeconds() << std::endl;
		window.draw(grid);
		d.onUpdate(elapsed.asSeconds());

		if (!once) {
			std::vector<sf::Vector2f> pts = d.getPointPairs();
			for (int i = 0; i < pts.size(); i += 2) {
				sf::Vector2f ep1 = pts[i];
				sf::Vector2f center = sf::Vector2f(d.getRadius(), d.getRadius());

				float x_diff = ep1.x - center.x;
				float y_diff = ep1.y - center.y;
				float slope = (y_diff) / (x_diff);

				float x_val = -1 * x_diff + d.getRadius();
				float calc = slope * (-1 * x_diff) + d.getRadius();

				float x_value = -x_diff; // distance from center
				float approx = slope * ((d.getRadius() + x_value) - d.getRadius()) + d.getRadius();


				std::printf("Center: (%f,%f)\n", center.x, center.y);
				std::cout << "slope: " << slope << std::endl;
				std::printf("Real: (%f,%f)\n", ep1.x, ep1.y);
				std::printf("Approximation: (%f,%f)\n", x_value, approx);
				//std::printf("(%f,%f)\n", x_val, calc);
				std::cout << std::endl;

				sf::VertexArray c_line(sf::Lines, 2);
				c_line[0].position = d.getOriginCoords() + ep1;
				c_line[0].color = sf::Color::Black;
				c_line[1].position = d.getOriginCoords() + sf::Vector2f(d.getRadius() + x_value, approx);
				c_line[1].color = sf::Color::Black;

				window.draw(c_line);

			}
			//once = true;
		}

		window.draw(d);
		// Update domains & render them
		//for (unsigned int i = 0; i < dList.size(); i++) {
		//	Domain* cur = dList.get(i);

		//	if (cur->isConsumed()) {
		//		dList.remove(i);
		//		//domainText.erase(domainText.begin() + i--);
		//	} else {
		//		cur->onUpdate(elapsed.asSeconds());
		//		/*domainText[i].setPosition(cur->getCenterCoords() + sf::Vector2f(-1 * domainText[i].getLocalBounds().getSize().x / 2, cur->getRadius() + cur->getOutlineThickness() + 10));
		//		domainText[i].setFillColor(cur->getColor());*/

		//		dList.overlapSearch(i);
		//		window.draw(*cur, camera);
		//		//window.draw(domainText[i], camera);
		//	}

		//}

		//window.draw(rect);
		window.display();
	}

	return 0;
}