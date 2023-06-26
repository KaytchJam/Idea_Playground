#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <utility>

bool RIGHT_HELD = false;
bool LEFT_HELD = false;

void printVector2f(sf::Vector2f& v) {
	std::printf("{%f,%f}\n", v.x, v.y);
}

void printVector2u(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void printVector2i(sf::Vector2u& v) {
	std::printf("{%d,%d}\n", v.x, v.y);
}

void draw_vector(std::vector<int>& v, sf::RenderWindow& w, float bl, float mh, float ml, bool print) {
	int NUM_ELEMENTS = v.size();
	int UPPER = NUM_ELEMENTS;

	for (int i = 0; i < v.size(); i++) {
		float value = v[i];
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

void draw_index(sf::RenderWindow& w, int n, int index, int value, float bl, float mh, float ml, sf::Color c) {
	float bh = (float)value / n * mh;
	sf::RectangleShape indexRect(sf::Vector2f(bl, bh));
	sf::Vector2f shift((float)index / n * ml, mh - bh);

	indexRect.setPosition(shift);
	indexRect.setFillColor(c);
	w.draw(indexRect);
}

// Function for Selection sort
void selectionSort(sf::RenderWindow& w, std::vector<int>& arr) {
	int i, j, min_idx;

	// One by one move boundary of
	// unsorted subarray
	for (i = 0; i < arr.size() - 1; i++) {

		// Find the minimum element in
		// unsorted array
		min_idx = i;
		for (j = i + 1; j < arr.size(); j++) {
			if (arr[j] < arr[min_idx])
				min_idx = j;
		}

		// Swap the found minimum element
		// with the first element
		/*if (min_idx != i) {
			int temp = arr[min_idx];
			arr[min_idx] = arr[i];
			arr[i] = temp;
		}*/
		std::swap(arr[min_idx], arr[i]);

	}
}

std::pair<int, int> select_and_sort(std::vector<int>& arr, int i) {
	int min_idx;

	// One by one move boundary of
	// unsorted subarray
	// Find the minimum element in
	// unsorted array
	min_idx = i;
	for (int j = i + 1; j < arr.size(); j++) {
		if (arr[j] < arr[min_idx])
			min_idx = j;
	}

	// Swap the found minimum element
	// with the first element
	return { min_idx, i };
}


void search_func(sf::RenderWindow& window, std::vector<int>& nums, std::pair<int, int>& block_pair, int index, int& jindex, int& min_idx, int block_len) {
	sf::Vector2u window_dim = window.getSize();

	if (nums[jindex] < nums[min_idx]) {
		min_idx = jindex;
		draw_vector(nums, window, block_len, window_dim.y, window_dim.x, true);
		draw_index(window, nums.size(), jindex, nums[jindex], block_len, window_dim.y, window_dim.x, sf::Color(0xBF, 0x57, 0x00));
		draw_index(window, nums.size(), index, nums[index], block_len, window_dim.y, window_dim.x, sf::Color(0xBF, 0x57, 0x00));
		draw_index(window, nums.size(), min_idx, nums[min_idx], block_len, window_dim.y, window_dim.x, sf::Color(0xCA, 0x80, 0xE9));
	}
	else {
		draw_vector(nums, window, block_len, window_dim.y, window_dim.x, true);
		draw_index(window, nums.size(), jindex, nums[jindex], block_len, window_dim.y, window_dim.x, sf::Color(0xBF, 0x57, 0x00));
		draw_index(window, nums.size(), index, nums[index], block_len, window_dim.y, window_dim.x, sf::Color(0xBF, 0x57, 0x00));
		draw_index(window, nums.size(), min_idx, nums[min_idx], block_len, window_dim.y, window_dim.x, sf::Color(0xCA, 0x80, 0xE9));
	}
	jindex++;
}

void pre_move_func(std::vector<int> nums, int& index, int& jindex, int& min_idx, int& DISTANCE, bool& MOVING_BLOCKS, std::pair<int, int>& block_pair, std::pair<int, int>& values) {
	block_pair = { min_idx, index };
	index++;
	jindex = index + 1;
	min_idx = index;
	MOVING_BLOCKS = true;

	int temp = block_pair.first;
	block_pair.first = std::min(temp, block_pair.second);
	block_pair.second = block_pair.first == temp ? block_pair.second : temp;

	values.first = nums[block_pair.first];
	values.second = nums[block_pair.second];

	nums[block_pair.first] = 0;
	nums[block_pair.second] = 0;

	DISTANCE = block_pair.second - block_pair.first;
}

void swap_and_reset(std::vector<int>& nums, std::pair<int, int>& block_pair, std::pair<int, int>& values, int& index, int& travelled, int& DISTANCE, bool& MOVING_BLOCKS) {
	nums[block_pair.first] = values.second;
	nums[block_pair.second] = values.first;

	travelled = 0;
	DISTANCE = 0;

	MOVING_BLOCKS = false;
	if (index == nums.size()) ++index;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "SORT ANIMATION!");

	const unsigned int NUM_ELEMENTS = 100;
	std::vector<int> nums(NUM_ELEMENTS);
	for (int i = 1; i <= NUM_ELEMENTS; i++)
		nums[i - 1] = i;

	std::srand(unsigned(std::time(0)));
	std::random_shuffle(nums.begin(), nums.end());

	window.setFramerateLimit(30);
	window.setKeyRepeatEnabled(false);

	bool START_SORT = false;
	bool PRINTED_ONCE = true;
	bool MOVING_BLOCKS = false;
	bool SELECTION_COMPLETE = false;
	bool SKIP = true;

	sf::Vector2u window_dim = window.getSize();
	float block_len = window_dim.x / nums.size();
	int UPPER = NUM_ELEMENTS + 1;
	int index = 0;
	int jindex = index + 1;
	int min_idx = index;

	std::pair<int, int> block_pair;
	std::pair<int, int> values;
	int DISTANCE = 0;
	int travelled = 0;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				START_SORT = true;
			}
		}

		window.clear();
		if (START_SORT && index <= nums.size()) {
			if (!MOVING_BLOCKS && index < nums.size()) {
				if (jindex < nums.size()) {
					search_func(window, nums, block_pair, index, jindex, min_idx, block_len);
				}
				else {
					if (min_idx == index) {
						std::printf("MIN_IDX(%d) == IDX(%d)\n", min_idx, index);
					}
					pre_move_func(nums, index, jindex, min_idx, DISTANCE, MOVING_BLOCKS, block_pair, values);
					draw_vector(nums, window, block_len, window_dim.y, window_dim.x, PRINTED_ONCE);
				}
			} else if (index <= nums.size()) {
				if (!SKIP && travelled < DISTANCE) {
					//std::cout << "travelled: " << travelled << std::endl;
					draw_vector(nums, window, block_len, window_dim.y, window_dim.x, PRINTED_ONCE);
					draw_index(window, nums.size(), block_pair.first + travelled, values.first, block_len, window_dim.y, window_dim.x, sf::Color(0xBF, 0x57, 0x00));
					draw_index(window, nums.size(), block_pair.second - travelled, values.second, block_len, window_dim.y, window_dim.x, sf::Color(0xCA, 0x80, 0xE9));
					++travelled;
				} else {
					swap_and_reset(nums, block_pair, values, index, travelled, DISTANCE, MOVING_BLOCKS);
					draw_vector(nums, window, block_len, window_dim.y, window_dim.x, PRINTED_ONCE);
				}
			} else {
				draw_vector(nums, window, block_len, window_dim.y, window_dim.x, PRINTED_ONCE);
			}
		}
		else {
			draw_vector(nums, window, block_len, window_dim.y, window_dim.x, PRINTED_ONCE);
		}
		PRINTED_ONCE = true;
		window.display();
	}

	return 0;
}