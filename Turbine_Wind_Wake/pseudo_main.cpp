#include <iostream>
#include <vector>
#include "VectorOutOverloads.h"
#include <sstream>
#include <string>

const char TURBINE_NONE = '-';
const char TURBINE_BUFFER = '+';
const char TURBINE_PRESENT = 'o';

struct uint_pair { unsigned int r, c; };

std::vector<std::vector<int>> generate_wind_grid(int* arr, unsigned int arr_len, unsigned int wg_len) {
	std::vector<int> src(arr, arr + arr_len);
	std::vector<std::vector<int>> wind_grid(wg_len);
	for (int i = 0; i < wind_grid.size(); i++) wind_grid[i] = src;
	return wind_grid;
}

std::vector<std::vector<char>> generate_turbine_grid(unsigned int arr_len, unsigned int tg_len) {
	std::vector<char> rowed_up(arr_len, TURBINE_NONE);
	std::vector<std::vector<char>> tg(tg_len, rowed_up);
	return tg;
}

bool in_range(int r, int c, std::vector<std::vector<char>>& arr) {
	return (r >= 0 && r < arr.size()) && (c >= 0 && c < arr[0].size());
}

bool update_turbine_grid(std::vector<std::vector<char>>& tg, unsigned int row, unsigned int col) {
	bool update_made = false;
	struct uint_pair indices[] = { {row, col + 1}, {row + 1, col}, {row - 1, col}, {row, col - 1} };
	tg[row][col] = TURBINE_PRESENT;

	for (int i = 0; i < 4; i++) {
		int r_cur = indices[i].r;
		int c_cur = indices[i].c;

		if (in_range(r_cur, c_cur, tg)) {
			tg[r_cur][c_cur] = TURBINE_BUFFER;
			update_made = true;
		}
	}
	return update_made;
}

int update_wind_grid(std::vector<std::vector<int>>& wg, unsigned int row, unsigned int col) {
	int energy_return = wg[row++][col];

	int wake = std::max(0, energy_return - 1);
	while (row < wg.size()) {
		wg[row++][col] = wake;
	}

	return energy_return;
}

int place_turbine(std::vector<std::vector<int>>& wg, std::vector<std::vector<char>>& tg, unsigned int row, unsigned int col) {
	if (row >= wg.size() || col >= wg[0].size()) return 0;

	int energy_gained = 0;
	// modify icons on tg
	if (tg[row][col] == TURBINE_NONE) {
		tg[row][col] = TURBINE_PRESENT;
		update_turbine_grid(tg, row, col);
		energy_gained = update_wind_grid(wg, row, col);
	}
	return energy_gained;
}

void print_wind_grid(std::vector<std::vector<int>>& wg) {
	std::string s;
	for (int i = 0; i < wg.size(); i++) {
		std::vector<int>& row = wg[i];
		for (int j = 0; j < row.size(); j++) {
			s += std::to_string(row[j]) + ' ';
		}
		s += '\n';
	}

	std::cout << s << std::endl;
}

void print_turbine_grid(std::vector<std::vector<char>>& wg) {
	std::string s;
	for (int i = 0; i < wg.size(); i++) {
		std::vector<char>& row = wg[i];
		for (int j = 0; j < row.size(); j++) {
			s += row[j];
			s += ' ';
		}
		s += '\n';
	}

	std::cout << s << std::endl;
}

int fill_grid(std::vector<std::vector<int>>& wg, std::vector<std::vector<char>>& tg) {
	int total_gain = 0;
	for (int i = 0; i < wg.size(); i++) {
		for (int j = 0; j < wg[0].size(); j++) {
			total_gain += place_turbine(wg, tg, i, j);
		}
	}
	return total_gain;
}

int playground() {
	using namespace std;

	int arr[] = { 3, 3, 3, 3, 3 };
	const unsigned int wg_length = 6;
	const unsigned int arr_size = sizeof(arr) / sizeof(int);
	vector<vector<int>> wind_grid = generate_wind_grid(arr, arr_size, wg_length);
	vector<vector<char>> turbine_grid = generate_turbine_grid(arr_size, wg_length);

	int total_gain = fill_grid(wind_grid, turbine_grid);

	// print our grids + energy
	print_wind_grid(wind_grid);
	print_turbine_grid(turbine_grid);
	cout << "Energy in the system : " << total_gain << endl;

	return 0;
}

int omain() {
	std::cout << "Hello World!" << std::endl;
	std::cout << playground() << std::endl;
	return 0;
}