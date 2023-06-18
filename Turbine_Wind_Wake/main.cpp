#include <iostream>
#include <vector>
#include "VectorOutOverloads.h"
#include <sstream>
#include <string>

const char TURBINE_NONE = '-';
const char TURBINE_BUFFER = '+';
const char TURBINE_PRESENT = 'o';

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

bool place_turbine(std::vector<int>& wg, std::vector<char>& tg, unsigned int col) {
	if (col >= wg.size()) return false;

	return true;
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

int playground() {
	using namespace std;

	int arr[] = { 5, 7, 2, 3, 3};
	const unsigned int wg_length = 6;
	vector<vector<int>> wind_grid = generate_wind_grid(arr, 5, wg_length);
	vector<vector<char>> turbine_grid = generate_turbine_grid(5, wg_length);

	// print our grids 
	print_wind_grid(wind_grid);
	print_turbine_grid(turbine_grid);

	return 0;
}

int main() {
	std::cout << "Hello World!" << std::endl;
	std::cout << playground() << std::endl;
	return 0;
}