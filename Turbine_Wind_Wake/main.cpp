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

sf::VertexArray make_line_strip(sf::RenderWindow& w, float length, float height) {

	float x = w.getSize().x / 2 - length / 2;
	float y = w.getSize().y / 2 - height / 2;

	sf::VertexArray gridborder(sf::LineStrip, 5);
	gridborder[0].position = sf::Vector2f(x, y);
	gridborder[1].position = sf::Vector2f(x + length, y);
	gridborder[2].position = sf::Vector2f(x + length, y + height);
	gridborder[3].position = sf::Vector2f(x, y + height);
	gridborder[4].position = sf::Vector2f(x, y);
	return gridborder;
}

sf::VertexArray make_grid_lines(sf::RenderWindow& w, int rows, int cols, float length, float height) {
	float x = w.getSize().x / 2 - length / 2;
	float y = w.getSize().y / 2 - height / 2;
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

sf::VertexArray make_simple_lines(sf::RenderWindow& w) {
	sf::VertexArray gridlines(sf::Lines, 4);
	gridlines[0].position = sf::Vector2f(200, 200);
	gridlines[1].position = sf::Vector2f(500, 200);

	gridlines[2].position = sf::Vector2f(200, 400);
	gridlines[3].position = sf::Vector2f(500, 400);
	return gridlines;

}

sf::VertexArray make_triangle_strip(sf::RenderWindow& w, float length, float height) {
	float x = w.getSize().x / 2 - length / 2;
	float y = w.getSize().y / 2 - height / 2;

	sf::VertexArray gridborder(sf::TriangleStrip, 4);
	gridborder[0].position = sf::Vector2f(x, y);
	gridborder[1].position = sf::Vector2f(x + length, y);
	gridborder[2].position = sf::Vector2f(x, y + height);
	gridborder[3].position = sf::Vector2f(x + length, y + height);
	return gridborder;
}

float calc_long_bar_length(int frac, int x) {
	return (1 - frac - frac) * x;
}

std::vector<sf::RectangleShape> make_rectangle_border(sf::RenderWindow &w) {
	std::vector<sf::RectangleShape> border(4);
	float win_length = w.getSize().x;
	float win_height = w.getSize().y;
	float bar_thickness = 20.f / 720.f * win_height;

	border[0] = sf::RectangleShape(sf::Vector2f((win_length * 18 / 20), bar_thickness));
	border[0].setPosition(sf::Vector2f(win_length * 1 / 20, win_height * 1 / 10));

	border[1] = sf::RectangleShape(sf::Vector2f(bar_thickness, win_height * 8 / 10));
	border[1].setPosition(sf::Vector2f(win_length * 19 / 20 - border[1].getSize().x, win_height * 1 / 10));
	//border[1].setFillColor(sf::Color::Red);
		
	border[2] = border[0];
	border[2].setPosition(sf::Vector2f(win_length * 1 / 20, win_height * 9 / 10 - border[0].getSize().y));
	//border[2].setFillColor(sf::Color::Blue);

	border[3] = border[1];
	border[3].setPosition(sf::Vector2f(win_length * 1 / 20, win_height * 1 / 10));
	//border[3].setFillColor(sf::Color::Green);

	return border;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SORT ANIMATION!");
	///sf::CircleShape shape(300.f);
	//sf::RectangleShape shape(sf::Vector2f(200.f, 200.f));
	//shape.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));

	sf::Vector2i dim(5, 10);
	sf::VertexArray strip = make_line_strip(window, 800, 600);
	sf::VertexArray lines = make_grid_lines(window, dim.x, dim.y, 800, 600);


	sf::Transform entity = sf::Transform::Identity;
	sf::RenderStates states;
	sf::Texture stemTxt, bladeTxt;
	sf::Sprite stemSprite, bladeSprite;

	if (!stemTxt.loadFromFile("assets/sprites/WindTurbineStem.png")) {
		std::cout << "failed to load: WindTurbineStem.png" << std::endl;
	}

	if (!bladeTxt.loadFromFile("assets/sprites/WindTurbineBlades.png")) {
		std::cout << "failed to laod: WindTurbineBlades.png" << std::endl;
	}



	std::vector<sf::RectangleShape> border = make_rectangle_border(window);

	float BLADE_OFFSET_X = 114.007f;
	float BLADE_OFFSET_Y = 427.352f;
	float BLADE_TO_CENTER_X = 325.015;
	float BLADE_TO_CENTER_Y = 369.495f;
	sf::Vector2f centerOffset(167.383f, 190.564f);
	float ORIGINAL_LENGTH = 638.965f;
	float ORIGINAL_HEIGHT = 591.076f;

	float XFACTOR = BLADE_TO_CENTER_X / ORIGINAL_LENGTH;
	float YFACTOR = BLADE_TO_CENTER_Y / ORIGINAL_HEIGHT;

	stemSprite.setTexture(stemTxt);
	stemSprite.scale(sf::Vector2f(0.25f, 0.25f));
	stemSprite.setPosition(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2) + sf::Vector2f(200.f, -25.f));

	std::cout << "Blade sprite scale: (" << bladeSprite.getScale().x << "," << bladeSprite.getScale().y << ")" << std::endl;
	bladeSprite.setTexture(bladeTxt);
	bladeSprite.scale(sf::Vector2f(0.25f, 0.25f));
	bladeSprite.setPosition(stemSprite.getPosition() + sf::Vector2f(BLADE_OFFSET_X / 2, 0.f));

	std::cout << "Blade texture size: (" << bladeTxt.getSize().x << "," << bladeTxt.getSize().y << ")" << std::endl;
	std::cout << "Blade sprite position: (" << bladeSprite.getPosition().x << "," << bladeSprite.getPosition().y << ")" << std::endl;
	std::cout << "Blade sprite size: (" << bladeTxt.getSize().x * bladeSprite.getScale().x << "," << bladeTxt.getSize().y * bladeSprite.getScale().y << ")" << std::endl;
	std::cout << "Blade sprite scale: (" << bladeSprite.getScale().x << "," << bladeSprite.getScale().y << ")" << std::endl;
	std::cout << "Blade sprite origin: (" << bladeSprite.getOrigin().x << "," << bladeSprite.getOrigin().y << ")" << std::endl;

	sf::Vector2f base_position = bladeSprite.getPosition();

	sf::RectangleShape test(sf::Vector2f(10.f, 10.f));
	test.setPosition(bladeSprite.getPosition() + centerOffset);
	test.setFillColor(sf::Color::Red);

	test.setOrigin(test.getSize() - sf::Vector2f(test.getSize().x / 2, test.getSize().y / 2));
	bladeSprite.setOrigin(XFACTOR * bladeTxt.getSize().x, YFACTOR * bladeTxt.getSize().y);
	///bladeSprite.setOrigin(bladeSprite.getOrigin().x + centerOffset.x * 2, bladeSprite.getOrigin().y + centerOffset.y * 2);

	sf::Clock dtClock;

	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);

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

		sf::Time elapsed = dtClock.restart();
		float deltaTime = elapsed.asSeconds();

		bladeSprite.rotate(50.f * deltaTime);


		//window.draw(strip, states);
		//window.draw(lines, states);
		window.draw(stemSprite);
		window.draw(bladeSprite);

		for (int i = border.size() - 1; i >= 0; i--) {
			window.draw(border[i]);
		}

		window.display();
	}

	return 0;
}