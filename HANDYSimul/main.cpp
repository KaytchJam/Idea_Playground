#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "Populations.h"
#include "MyTools/MyV.h"
#include "CustomShapes/ColumnShape.h"

#define M_PI 3.14159265358979323846

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

static sf::Vector2f lerp(const sf::Vector2f start, const sf::Vector2f end, const int numerator, const int denominator) {
	float frac = (float) numerator / denominator;
	return start * (1 - frac) + end * (frac);
}

int renderLoop() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);

	ColumnShape col(20.f, 80.f);
	col.setPosition(mid + sf::Vector2f(0 - 10.f, -80.f));

	std::vector<float> heights = {80.f, 100.f, 60.f, 300.f, 250.f, 180.f, 140.f, 80.f};
	const int FRAME_DELAY = 20;
	int height_index = 0;
	int frames_passed = 0;

	int frame = 0;
	lalg::vec4 v1 = { 0x1E, 0xBA, 0xE1, 0xFF};
	//lalg::mat4 r1 = lalg::rotationMatY(30 / 360 * 2 * M_PI);

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


		// MODIFY HEIGHT
		float l = lerp({ 0, col.getHeight() }, { 0, heights[height_index] }, frames_passed, FRAME_DELAY).y;
		printf("Current: %f, Target: %f\n", l, heights[height_index]);

		float prev_height = col.getHeight();
		col.setHeight(l);
		col.setPosition(col.getPosition() - sf::Vector2f(0, l - prev_height));

			// conditional increments
		bool move_on = frames_passed == (FRAME_DELAY - 1);
		height_index += move_on * 1;
		height_index *= !(height_index == heights.size());
		frames_passed = (frames_passed + 1) * !move_on;

		std::printf("Frames Passed: %d - Height Index %d\n\n", frames_passed, height_index);


		// RENDER
		window.clear(sf::Color(0xE1E1E1FF));
		window.draw(col);
		window.display();

		frame = (frame + 1) * !(frame == 359);
	}

	return 0;
}

int main() {
	//std::cout << "Hello world" << std::endl;
	//return runSim(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
	renderLoop();
}