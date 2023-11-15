#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "Populations.h"

int runSimToFile(float elite_total_pop, float commoner_total_pop, float nature_total_stock , float wealth_total_stock) {
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

	const int END = 1000;
	float t = 0;
	float prev = 0;

	std::ofstream myFile;
	myFile.open("output/simulation.csv");
	myFile << "timestep," << "elites," << "commoners," << "nature," << "wealth\n";

	// render - calculations loop
	while (t < END) {
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
		prev = t;
		t++;
	}

	myFile.close();
	std::cout << "Finished executing." << std::endl;
	return 0;
}

int runSim(float elite_total_pop, float commoner_total_pop, float nature_total_stock, float wealth_total_stock) {
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

	const int END = 1000;
	float t = 0;
	float prev = 0;

	// render - calculations loop
	while (t < END) {
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

		prev = t;
		t++;
	}

	std::cout << "Finished executing." << std::endl;
	return 0;
}

int renderLoop() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "");

	sf::CircleShape base(20.f);
	base.setPosition(sf::Vector2f(WIN_LENGTH / 2, WIN_HEIGHT / 2));
	base.setFillColor(sf::Color::Blue);

	// run the program as long as the window is open
	window.setFramerateLimit(30);
	while (window.isOpen()) {
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(0xE1, 0xE1, 0xE1));
		window.draw(base);
		window.display();
	}

	return 0;
}

int main() {
	//std::cout << "Hello world" << std::endl;
	//return runSim(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
	renderLoop();
}