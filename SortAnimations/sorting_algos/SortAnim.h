#pragma once
#include <SFML/Graphics.hpp>

// our basic SortAnimation object
class SortAnim {
public:
	SortAnim() {}
	virtual ~SortAnim() {}
	virtual void onUpdate() {}
	virtual void onRender() {}
};

static void draw_index(sf::RenderWindow& w, size_t n, int index, int value, float bl, float mh, float ml, sf::Color c) {
	float bh = (float)value / n * mh;
	sf::RectangleShape indexRect(sf::Vector2f(bl, bh));
	sf::Vector2f shift((float)index / n * ml, mh - bh);

	indexRect.setPosition(shift);
	indexRect.setFillColor(c);
	w.draw(indexRect);
}

static void draw_vector(std::vector<int>& v, sf::RenderWindow& w, float bl, float mh, float ml, bool print) {
	size_t NUM_ELEMENTS = v.size();
	size_t UPPER = NUM_ELEMENTS;

	for (int i = 0; i < v.size(); i++) {
		float value = (float)v[i];
		float block_height = (value) / (UPPER) * (mh);
		sf::RectangleShape numRect(sf::Vector2f(bl, block_height));
		sf::Vector2f shift(((float)i / NUM_ELEMENTS) * ml, mh - block_height);

		numRect.setPosition(shift);
		numRect.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));
		w.draw(numRect);

		if (!print) {
			std::printf("index: %d value: %f len: %f het: %f\n", i, value, bl, block_height);
			std::printf("x: %f, y: %f\n\n", shift.x, shift.y);
		}
	}
}