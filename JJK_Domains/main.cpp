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

sf::Vector2f getCircleCenter(sf::CircleShape c) {
	sf::Vector2f pt = c.getPoint(0);
	std::cout << pt.x << " " << pt.y << std::endl;
	return sf::Vector2f(c.getPoint(0).x, c.getPoint(0).y + c.getRadius());
}

sf::Vector2f getCircleCenter2(sf::CircleShape c) {
	sf::Vector2f pos = c.getPosition();
	return pos + sf::Vector2f(c.getRadius(), c.getRadius());
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "SORT ANIMATION!");
	///sf::CircleShape shape(300.f);
	//sf::RectangleShape shape(sf::Vector2f(200.f, 200.f));
	//shape.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));

	sf::Vector2i dim(5, 10);
	
	/*sf::VertexArray shape = make_line_strip(window, 800, 600);
	sf::VertexArray lines = make_grid_lines(window, dim.x, dim.y, 800, 600);*/
	sf::CircleShape circ(300.0f);
	circ.setOutlineThickness(10.f);
	circ.setFillColor(sf::Color::Transparent);
	circ.setOutlineColor(sf::Color::Red);

	//circ.scale(sf::Vector2f(5.f, 5.f));

	circ.move(sf::Vector2f(10.f, 10.f));
	circ.move(sf::Vector2f(window.getSize().x / 2 - circ.getRadius() - circ.getOutlineThickness(), window.getSize().y / 2 - circ.getRadius() - circ.getOutlineThickness()));
	sf::Vector2f center = getCircleCenter2(circ);
	std::printf("Origin: (%f, %f)\n", circ.getOrigin().x, circ.getOrigin().y);
	std::printf("Pos: (%f, %f)\n", circ.getPosition().x, circ.getPosition().y);
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

		window.clear(sf::Color(0xFF, 0xFF, 0xFF));

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

		/*window.draw(shape, states);
		window.draw(lines, states);*/
		window.draw(circ, states);
		window.display();
	}

	return 0;
}