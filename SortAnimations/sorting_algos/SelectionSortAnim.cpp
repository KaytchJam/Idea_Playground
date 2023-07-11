#include "SelectionSortAnim.h"
#include "../globals/KeyState.h"
#include <iostream>

extern struct mousestates mouse_states;

SelectionSortAnim::SelectionSortAnim(sf::RenderWindow& w, std::vector<int>& v) : w(w), v(v) {
	max_length = (float) w.getSize().x;
	max_height = (float) w.getSize().y;
	box_length =  (float) 1 / v.size() * max_length;
}

void SelectionSortAnim::update_min_index() {
	SEARCHING = true;
	jindex++;

	if (v[jindex] < v[mindex]) {
		mindex = jindex;
	}
}

void SelectionSortAnim::swap_and_reset() {
	std::swap(v[mindex], v[index]);

	index++;
	jindex = index;
	mindex = index;
	SEARCHING = false;

	if (index == v.size()) ++index;
}

void SelectionSortAnim::onUpdate() {
	if (mouse_states.MOUSE_HELD) START_SORT = true; 

	if (START_SORT && index < v.size()) {
		if (jindex < v.size() - 1) update_min_index();
		else swap_and_reset();
	} else {
		SEARCHING = false;
	}
}

void SelectionSortAnim::onRender() {
	draw_vector(v, w, box_length, max_height, max_length, true);
	if (SEARCHING) {
		draw_index(w, v.size(), jindex, v[jindex], box_length, max_height, max_length, sf::Color(0xBF, 0x57, 0x00));
		draw_index(w, v.size(), index, v[index], box_length, max_height, max_length, sf::Color(0xBF, 0x57, 0x00));
		draw_index(w, v.size(), mindex, v[mindex], box_length, max_height, max_length, sf::Color(0xCA, 0x80, 0xE9));
	}
}

