//#pragma once
//#include "SortAnim.h"
//
//static void draw_index(sf::RenderWindow& w, int n, int index, int value, float bl, float mh, float ml, sf::Color c) {
//	float bh = (float)value / n * mh;
//	sf::RectangleShape indexRect(sf::Vector2f(bl, bh));
//	sf::Vector2f shift((float)index / n * ml, mh - bh);
//
//	indexRect.setPosition(shift);
//	indexRect.setFillColor(c);
//	w.draw(indexRect);
//}
//
//static void draw_vector(std::vector<int>& v, sf::RenderWindow& w, float bl, float mh, float ml, bool print) {
//	int NUM_ELEMENTS = v.size();
//	int UPPER = NUM_ELEMENTS;
//
//	for (int i = 0; i < v.size(); i++) {
//		float value = v[i];
//		float block_height = (value) / (UPPER) * (mh);
//		sf::RectangleShape numRect(sf::Vector2f(bl, block_height));
//		sf::Vector2f shift(((float)i / NUM_ELEMENTS) * ml, mh - block_height);
//
//		numRect.setPosition(shift);
//		numRect.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));
//		w.draw(numRect);
//
//		if (!print) {
//			std::printf("index: %d value: %f len: %f het: %f\n", i, value, bl, block_height);
//			std::printf("x: %f, y: %f\n\n", shift.x, shift.y);
//		}
//	}
//}
//
//class SelectionSortAnim : public SortAnim {
//public:
//	SelectionSortAnim(sf::RenderWindow*& w, std::vector<int>& v);
//	~SelectionSortAnim();
//	void onUpdate();
//	void onRender();
//
//private:
//	sf::RenderWindow*& w;
//	std::vector<int>& v;
//
//	float max_length;
//	float max_height;
//	float box_length;
//
//	bool START_SORT = false;
//	bool SEARCHING = false;
//	//bool SHOW_SWAP = false;
//
//	unsigned int index = 0;
//	unsigned int jindex = index;
//	unsigned int mindex = index;
//	/*int DISTANCE = 0;
//	int travelled = 0;*/
//
//	/*std::pair<int, int> box_pair;
//	std::pair<int, int> values;*/
//
//	void update_min_index();
//	//void set_box_pair();
//	void swap_and_reset();
//};