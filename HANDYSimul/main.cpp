#include <iostream>
#include <stdlib.h>
#include <Windows.h>

#include "Populations.h"

int main() {
	std::cout << "Hello world" << std::endl;

	struct human_pop elites;
	struct human_pop commoners;
	struct nature_stock nature;
	struct wealth_stock wealth;

	// initializing each of our "stocks"
	init_human_pop(elites, 100.f, 0.5f, 1.5f, 8.f, 0.2f);
	init_human_pop(commoners, 800.f, 0.5f, 1.f, 0.f, 0.2f);
	init_nature(nature, 2000.f, 5.f, 1.2f, 4000.f);
	init_wealth(wealth, 200.f, 50.f);

	const int END = 100;
	int t = 0;

	// render - calculations loop
	while (t < END) {
		system("cls");
		printf("Iteration: %d\n", t++);
		Sleep(500);
	}

	std::cout << "Finished executing." << std::endl;
	return 0;
}