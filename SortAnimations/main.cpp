#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

#include "globals/KeyState.h"
#include "sorting_algos/SelectionSortAnim.h"

extern struct keystates key_states;
extern struct mousestates mouse_states;

void printVector2f(sf::Vector2f& v) {
	std::printf("{%f,%f}\n", v.x, v.y);
}

void printVector2u(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void printVector2i(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "sort_animation");

	const unsigned int NUM_ELEMENTS = 150;
	std::vector<int> nums(NUM_ELEMENTS);
	for (int i = 1; i <= NUM_ELEMENTS; i++)
		nums[i - 1] = i;

	std::srand(unsigned(std::time(0)));
	std::random_shuffle(nums.begin(), nums.end());

	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);

	SortAnim* s = new SelectionSortAnim(window, nums);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::MouseButtonPressed) mouse_states.MOUSE_HELD = true;
			if (event.type == sf::Event::MouseButtonReleased) mouse_states.MOUSE_HELD = false;
		}

		window.clear();
		s->onUpdate();
		s->onRender();
		window.display();
	}

	return 0;
}