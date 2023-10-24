#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <iomanip>

#include "entity/domain_types/Domain.h"
#include "entity/domain_types/DomainManager.h"
#include "globals/UserListener.h"

#include "ui_stuffs/UIButton.h"

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

void held_toggle(sf::Event& event) {
	switch (event.mouseButton.button) {
	case sf::Mouse::Button::Left:
		user_mouse.LEFT_HELD = true;
		user_mouse.LEFT_CLICK_POSITION = user_mouse.position;
		break;
	case sf::Mouse::Button::Right:
		user_mouse.RIGHT_HELD = true;
		user_mouse.RIGHT_CLICK_POSITION = user_mouse.position;
		break;
	}

	user_mouse.MOUSE_HELD = true;
}

void mouse_toggle(sf::Event& event, bool held, bool released, bool holding) {
	switch (event.mouseButton.button) {
	case sf::Mouse::Left:
		user_mouse.LEFT_HELD = held;
		user_mouse.LEFT_RELEASED = released;
		user_mouse.HOLDING_OBJECT = holding;
		break;
	case sf::Mouse::Right:
		user_mouse.RIGHT_HELD = held;
		user_mouse.RIGHT_RELEASED = released;
		break;
	}

	user_mouse.MOUSE_HELD = user_mouse.LEFT_HELD || user_mouse.RIGHT_HELD;
	//user_mouse.MOUSE_RELEASED = user_mouse.RIGHT_RELEASED && user_mouse.LEFT_RELEASED;
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
	if (!font.loadFromFile("assets/fonts/georgia.ttf"))
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
	DomainManager dList;
	{
		ClosedDomain d1(100.f, sf::Color::Blue, 1.3f, ClosedDomain::centerToOriginCoords(sf::Vector2f((float)window.getSize().x / 2, (float)window.getSize().y / 2), 150.f));
		OpenDomain d2(180.f, sf::Color::Red, 1.3f, Domain::centerToOriginCoords(d1.getCenterCoords(), 120));
		dList.add(d1);
		dList.add(d2);
	}

	struct DomainData {
		DomainType dt = DomainType::ENUM_SIZE;
		float radius = 0.f;
		sf::Color c;
		float refinement = 0.f;
		sf::Vector2f position;
	};

	struct DomainData dd;
	dd.dt = DomainType::CLOSED_DOMAIN;
	dd.radius = 100.f;
	dd.c = sf::Color::Black;
	dd.refinement = 1.0f;
	dd.position = sf::Vector2f(0.f, 0.f);

	const void* vptr[2] = { NULL, NULL };
	vptr[0] = (void*)&dList;
	vptr[1] = (void*)&dd;

	UIButton button1(sf::Vector2f(100.f, 50.f), vptr, 2, [](const void** args, const unsigned int NUM_ARGS) {
		DomainManager* dm = (DomainManager*) (args[0]);
		DomainData* d = (DomainData*) (args[1]);
		dm->add(d->dt, d->radius, d->c, d->refinement, d->position);
	});

	button1.setPosition(sf::Vector2f(100.f, window.getSize().y - 100.f));
	button1.setFont(font);
	button1.setString("MAKE");
	button1.setButtonColor(sf::Color::Black);
	//button1.setTextColor(sf::Color::White);

	std::cout << "Button1 position" << std::endl;
	std::cout << "(" << button1.getPosition().x << "," << button1.getPosition().y << ")" << std::endl;
	std::cout << "button initialized" << std::endl;

	sf::Transform entity = sf::Transform::Identity;
	sf::RenderStates camera;

	OpenDomain d(100.f, sf::Color::Black, 2.f, sf::Vector2f(300.f, 300.f));

	std::vector<sf::Text> domainText(dList.size());
	int index = 0;
	for (sf::Text& dText : domainText) {
		Domain* cur = dList.get(index);
		dText.setFont(font);
		dText.setCharacterSize(24);
		dText.setFillColor(cur->getColor());
		dText.setPosition(cur->getCenterCoords() + sf::Vector2f(0, cur->getRadius() + cur->getOutlineThickness() + 10));
		//dText.setString("D " + std::to_string(index + 1) /*+ "\nRef: " + std::to_string(cur->getRefinement())*/);
		std::ostringstream oss;
		oss << std::setprecision(2) << cur->getRefinement();
		dText.setString("r: " + oss.str());

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
		user_mouse.LEFT_RELEASED = false;
		user_mouse.RIGHT_RELEASED = false;
		user_mouse.position = sf::Mouse::getPosition(window);

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			else if (event.type == sf::Event::KeyPressed) { direction_toggle(event, true); }
			else if (event.type == sf::Event::KeyReleased) { direction_toggle(event, false); }
			else if (event.type == sf::Event::MouseButtonPressed) { held_toggle(event); }
			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					user_mouse.LEFT_HELD = false;
					user_mouse.LEFT_RELEASED = true;
					user_mouse.HOLDING_OBJECT = false;
				} else if (event.mouseButton.button == sf::Mouse::Right) {
					user_mouse.RIGHT_HELD = false;
					user_mouse.RIGHT_RELEASED = true;
				}
				user_mouse.MOUSE_RELEASED = true;
			}
		}

		//sf::Vector2i localPosition = sf::Mouse::getPosition(window);
		//printVector2i(localPosition);

		window.clear(sf::Color(0xE1, 0xE1, 0xE1));

		sf::Time elapsed = dtClock.restart();

		//std::cout << "deltaTime: " << elapsed.asSeconds() << std::endl;
		window.draw(grid);
		button1.onUpdate(elapsed.asSeconds());

		// Update domains & render them
		for (unsigned int i = 0; i < dList.size(); i++) {
			Domain* cur = dList.get(i);

			if (cur->isConsumed()) {
				dList.remove(i);
				//domainText.erase(domainText.begin() + i--);
			} else {
				cur->onUpdate(elapsed.asSeconds());
				//domainText[i].setPosition(cur->getCenterCoords() + sf::Vector2f(-1 * domainText[i].getLocalBounds().getSize().x / 2, cur->getRadius() + cur->getOutlineThickness() + 10));
				//domainText[i].setFillColor(cur->getColor());

				dList.overlapSearch(i);
				window.draw(*cur, camera);
				//window.draw(domainText[i], camera);
			}

		}

		window.draw(button1);
		window.display();
	}

	return 0;
}