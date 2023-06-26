#pragma once
#include "SortAnim.h"

class SelectionSortAnim : public SortAnim {
public:
	SelectionSortAnim();
	~SelectionSortAnim();

	void onUpdate();
	void onRender();

private:
	sf::RenderWindow& w;
	std::vector<int>& v;

	bool START_SORT = false;
	bool SEARCHING = false;

	unsigned int index = 0;
	unsigned int search_index = 0;
	unsigned int middle_index = 0;
};