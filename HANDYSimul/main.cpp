#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "Populations.h"

int run(float elite_total_pop, float commoner_total_pop, float nature_total_stock , float wealth_total_stock) {
	// populations
	struct human_pop elites;
	struct human_pop commoners;
	struct nature_stock nature;
	struct wealth_stock wealth;

	// initializing each of our "stocks"
	init_human_pop(elites, elite_total_pop, BASE_ELITE_BIRTH_RATE, BASE_INEQUALITY_FACTOR, BASE_TWPC, BASE_SSPC);
	init_human_pop(commoners, commoner_total_pop, BASE_COMMONER_BIRTH_RATE, 1.f, BASE_TWPC, BASE_SSPC);
	init_nature(nature, nature_total_stock, BASE_REGENERATION_RATE, BASE_DEPLETION_FACTOR, BASE_NATURE_CARRY_CAPACITY);
	init_wealth(wealth, wealth_total_stock, 0.0f);

	const int END = 100;
	float t = 0;
	float prev = 0;

	std::ofstream myFile;
	myFile.open("output/simulation.csv");
	myFile << "timestep," << "elites," << "commoners," << "nature," << "wealth\n";

	// render - calculations loop
	while (t < END) {
		system("cls");

		// computing the change in population size d/dt
		calcFlow(elites);
		calcFlow(commoners);
		calcFlow(nature, commoners);
		calcFlow(wealth, nature, elites, commoners);

		// computing the solution
		calcStock(elites, prev, t, true);
		calcStock(commoners, prev, t, true);
		calcStock(nature, prev, t, true);
		calcStock(wealth, prev, t, true);

		myFile << t << ',' << elites.stock << ',' << commoners.stock << ','
			<< nature.stock << ',' << wealth.stock << '\n';
 
		// debugging & printing
		/*
		printf("Iteration: %f\n", t);
		printf("Elite Population: %f\n", elites.stock);
		printf("Commoner Population: %f\n", commoners.stock);
		printf("Nature Stock: %f\n", nature.stock);
		printf("Wealth Stock: %f\n", wealth.stock);

		std::cout << "Wealth threshold " << wealth.wealth_threshold << std::endl;
		std::cout << "Death rate " << elites.death_rate << std::endl;
		std::cout << "Commoner death rate " << commoners.death_rate << std::endl;
		std::wcout << "Elite death rate " << elites.death_rate << std::endl;
		std::cout << "Commoner birth rate " << commoners.birth_rate << std::endl;
		*/

		prev = t;
		t++;
		//Sleep(2000);
	}

	myFile.close();
	std::cout << "Finished executing." << std::endl;
	return 0;
}

int main() {
	std::cout << "Hello world" << std::endl;
	return run(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
}