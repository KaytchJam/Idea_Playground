//#include "SelectionSortAnim.h"
//
//SelectionSortAnim::SelectionSortAnim(sf::RenderWindow*& w, std::vector<int>& v) {
//	this->w = w;
//	this->v = v;
//
//	max_length = w->getSize().x;
//	max_height = w->getSize().y;
//	box_length =  1 / v.size() * max_length;
//}
//
//void SelectionSortAnim::update_min_index() {
//	SEARCHING = true;
//	jindex++;
//
//	if (v[jindex] < v[mindex]) {
//		mindex = jindex;
//	}
//}
//
////void SelectionSortAnim::set_box_pair(std::vector<int> nums, int& index, int& jindex, int& min_idx, int& DISTANCE, bool& MOVING_BLOCKS, std::pair<int, int>& block_pair, std::pair<int, int>& values) {
////	box_pair = { min_idx, index };
////	index++;
////	jindex = index + 1;
////	min_idx = index;
////	MOVING_BLOCKS = true;
////
////	int temp = block_pair.first;
////	block_pair.first = std::min(temp, block_pair.second);
////	block_pair.second = block_pair.first == temp ? block_pair.second : temp;
////
////	values.first = nums[block_pair.first];
////	values.second = nums[block_pair.second];
////
////	nums[block_pair.first] = 0;
////	nums[block_pair.second] = 0;
////
////	DISTANCE = block_pair.second - block_pair.first;
////}
//
//void SelectionSortAnim::swap_and_reset() {
//	index++;
//	jindex = index + 1;
//	mindex = index;
//	SEARCHING = false;
//
//	std::swap(v[mindex], v[index]);
//	if (index == v.size()) ++index;
//	draw_vector(v, *w, box_length, max_height, max_length, true);
//}
//
//void SelectionSortAnim::onUpdate() {
//	if (START_SORT && index <= v.size()) {
//		if (jindex < v.size()) update_min_index();
//		else swap_and_reset();
//	} else {
//		draw_vector(v, *w, box_length, max_height, max_length, true);
//	}
//}
//
//void SelectionSortAnim::onRender() {
//	if (SEARCHING) {
//		draw_vector(v, *w, box_length, max_height, max_length, true);
//		draw_index(*w, v.size(), jindex, v[jindex], box_length, max_height, max_length, sf::Color(0xBF, 0x57, 0x00));
//		draw_index(*w, v.size(), index, v[index], box_length, max_height, max_length, sf::Color(0xBF, 0x57, 0x00));
//		draw_index(*w, v.size(), mindex, v[mindex], box_length, max_height, max_length, sf::Color(0xCA, 0x80, 0xE9));
//	} else {
//		draw_vector(v, *w, box_length, max_height, max_length, true);
//	}
//}

