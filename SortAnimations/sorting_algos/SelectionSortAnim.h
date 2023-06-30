#pragma once
#include "SortAnim.h"
#include <SFML/Graphics.hpp>

class SelectionSortAnim : public SortAnim {
public:
	SelectionSortAnim(sf::RenderWindow& w, std::vector<int>& v);
	~SelectionSortAnim() {}
	void onUpdate();
	void onRender();

private:
	sf::RenderWindow& w;
	std::vector<int>& v;

	float max_length;
	float max_height;
	float box_length;

	bool START_SORT = false;
	bool SEARCHING = false;

	unsigned int index = 0;
	unsigned int jindex = index;
	unsigned int mindex = index;

	void update_min_index();
	void swap_and_reset();
};