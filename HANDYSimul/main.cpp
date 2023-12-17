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

void update_maxes(lalg::vec4& max_vector, lalg::vec4& cur_stocks) {
	const float hpop_max = std::max(cur_stocks.r, cur_stocks.g);
	max_vector.r = std::max(max_vector.r, hpop_max);
	max_vector.g = max_vector.r;
	max_vector.a = std::max(max_vector.a, cur_stocks.a);
}

static sf::Vector2f lerp(const sf::Vector2f start, const sf::Vector2f end, const int numerator, const int denominator) {
	float frac = (float) numerator / denominator;
	return start * (1 - frac) + end * (frac);
}

static lalg::vec4 lerp(const lalg::vec4 start, const lalg::vec4 end, const int numerator, const int denominator) {
	float frac = (float) numerator / denominator;
	return start * (1 - frac) + end * (frac);
}

// normalizes value in a range, assume RANGE.x is the lower bound and RANGE.y is the upper bound
static float normalize(const float val, const sf::Vector2f RANGE) {
	return (val - RANGE.x) / (RANGE.y - RANGE.x);
}

static lalg::vec4 normalize(const lalg::vec4& vals, const sf::Vector2f RANGE) {
	return (vals - RANGE.x * lalg::onesVec()) / (RANGE.y - RANGE.x);
}

static lalg::vec4 normalizeL(const lalg::vec4& vals, const lalg::vec4& maxes) {
	return lalg::map(lalg::diag(maxes), [](float f) { return 1 / (f + (f == 0.0f)) - (f == 0.0f); }) * vals;
}

static float normToRange(const float val, const sf::Vector2f OLD_RANGE, const sf::Vector2f NEW_RANGE) {
	return normalize(val, OLD_RANGE) * (NEW_RANGE.y - NEW_RANGE.x) + NEW_RANGE.x;
}

static lalg::vec4 sfToLalg(const sf::Vector2f& sfVec) {
	return { sfVec.x, sfVec.y, 1, 1 };
}

static sf::Vector2f lalgToSf(const lalg::vec4& v) {
	return { v.r, v.g };
}

int renderLoop(float elite_total_pop, float commoner_total_pop, float nature_total_stock, float wealth_total_stock) {

	// HANDY SIM INITIALIZATIONS
	struct human_pop elites;
	struct human_pop commoners;
	struct nature_stock nature;
	struct wealth_stock wealth;

	init_human_pop(elites, elite_total_pop, BASE_ELITE_BIRTH_RATE, BASE_INEQUALITY_FACTOR, BASE_TWPC, BASE_SSPC);
	init_human_pop(commoners, commoner_total_pop, BASE_COMMONER_BIRTH_RATE, 1.f, BASE_TWPC, BASE_SSPC);
	init_nature(nature, nature_total_stock, BASE_REGENERATION_RATE, BASE_DEPLETION_FACTOR, BASE_NATURE_CARRY_CAPACITY);
	init_wealth(wealth, wealth_total_stock, 0.0f);

	population* populist[4] = { &elites, &commoners, &nature, &wealth };

	// SFML INITIALIZATIONS
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);

	ColumnShape natureCol(20.f, 80.f);
	natureCol.setPosition(mid + sf::Vector2f(0 - 10.f, -80.f));
	natureCol.setColor(0x0, 0x88, 0x0);

	sf::CircleShape natureTri(20, 3);
	natureTri.setFillColor(sf::Color(0x008800FF));
	//tri.setPosition(natureCol.getPosition() + sf::Vector2f(0, natureCol.getHeight() + 60.f));

	sf::Vector2f p = natureTri.getPosition();
	std::printf("Origin: (%f, %f)\n", p.x, p.y);


	lalg::vec4 p1 = sfToLalg(natureTri.getPoint(0));
	lalg::vec4 p2 = sfToLalg(natureTri.getPoint(1));
	lalg::vec4 p3 = sfToLalg(natureTri.getPoint(2));

	sf::Vector2f mp = lalgToSf((p1 + p2 + p3) / 3);

	natureTri.setOrigin(mp);
	natureTri.move(natureCol.getPosition() + sf::Vector2f(0.f, natureCol.getHeight() + 60.f) + mp);
	//tri.rotate(180.f);
	//tri.move(sf::Vector2f(0, 30.f));
	//tri.rotate(180);

	ColumnShape wealthCol(20.f, 80.f);
	wealthCol.setPosition(mid + sf::Vector2f(0 - 10.f + 60.f, -80.f));
	wealthCol.setColor(0xDF, 0x88, 0x0);

	sf::CircleShape wealthTri(20, 3);
	wealthTri.setFillColor(sf::Color(0xDF8800FF));
	wealthTri.setOrigin(mp);
	wealthTri.move(wealthCol.getPosition() + sf::Vector2f(0.f, wealthCol.getHeight() + 60.f) + mp);
	//tri2.setPosition(wealthCol.getPosition() + sf::Vector2f(0, wealthCol.getHeight() + 60.f));

	ColumnShape commonerCol(20.f, 80.f);
	commonerCol.setPosition(mid + sf::Vector2f(0 - 10.f - 60.f, -80.f));
	commonerCol.setColor(0x0, 0x0, 0x88);

	sf::CircleShape commonTri(20, 3);
	commonTri.setFillColor(sf::Color(0x000088FF));
	commonTri.setOrigin(mp);
	commonTri.move(commonerCol.getPosition() + sf::Vector2f(0.f, commonerCol.getHeight() + 60.f) + mp);
	//tri3.setPosition(commonerCol.getPosition() + sf::Vector2f(0, commonerCol.getHeight() + 60.f));

	ColumnShape eliteCol(20.f, 80.f);
	eliteCol.setPosition(commonerCol.getPosition() + sf::Vector2f(-60.f, 0));
	eliteCol.setColor(0xBA, 0x0, 0x0);

	sf::CircleShape eliteTri(20, 3);
	eliteTri.setFillColor(sf::Color(0xBA0000FF));
	eliteTri.setOrigin(mp);
	eliteTri.move(eliteCol.getPosition() + sf::Vector2f(0.f, eliteCol.getHeight() + 60.f) + mp);
	//eliteTri.setPosition(eliteCol.getPosition() + sf::Vector2f(0, eliteCol.getHeight() + 60.f));


	// column height interpolation + computation rate
	std::vector<float> heights = {80.f, 100.f, 60.f, 300.f, 250.f, 180.f, 140.f, 80.f};
	const unsigned int FRAME_DELAY = 10;
	unsigned int height_index = 0;
	unsigned int frames_passed = 0;
	unsigned int frame = 0;

	unsigned int timestep = 0;

	float max_human_pop = commoner_total_pop * 10;
	lalg::vec4 max_vector = { max_human_pop, max_human_pop, nature.capacity, std::max(wealth_total_stock * 5, max_human_pop / 2)};
	// elite, commoner, nature, wealth
	float MAX_HEIGHT = 200;
	lalg::vec4 stockHeights = {elites.stock, commoners.stock, nature.stock, wealth.stock};
	lalg::printVec(stockHeights);
	lalg::printVec(max_vector);
	stockHeights = normalizeL(stockHeights, max_vector) * MAX_HEIGHT;

	// run the program as long as the window is open
	window.setFramerateLimit(30);
	while (window.isOpen()) {
		std::printf("timestep %d: ", timestep);
		printVec(stockHeights);

		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// computing change in stock size dS/dt
		calcFlow(elites);
		calcFlow(commoners);
		calcFlow(nature, commoners);
		calcFlow(wealth, nature, elites, commoners);

		// computing the solution, current stock S
		calcStock(elites, 0, 1, true);
		calcStock(commoners, 0, 1, true);
		calcStock(nature, 0, 1, true);
		calcStock(wealth, 0, 1, true);

		bool move_on = frames_passed == (FRAME_DELAY - 1);

		lalg::vec4 all_stocks = { elites.stock, commoners.stock, nature.stock, wealth.stock };

		update_maxes(max_vector, all_stocks);
		lalg::vec4 normed_stocks = normalizeL(all_stocks, max_vector);
		stockHeights = MAX_HEIGHT * normed_stocks;

		// MODIFY HEIGHT
		lalg::vec4 colHeights = { eliteCol.getHeight(), commonerCol.getHeight(), natureCol.getHeight(), wealthCol.getHeight()};

		eliteCol.setHeight(stockHeights.r);
		commonerCol.setHeight(stockHeights.g);
		natureCol.setHeight(stockHeights.b);
		wealthCol.setHeight(stockHeights.a);

		lalg::vec4 diffs = stockHeights - colHeights;
		eliteCol.setPosition(eliteCol.getPosition() - sf::Vector2f(0, diffs.r));
		commonerCol.setPosition(commonerCol.getPosition() - sf::Vector2f(0, diffs.g));
		natureCol.setPosition(natureCol.getPosition() - sf::Vector2f(0, diffs.b));
		wealthCol.setPosition(wealthCol.getPosition() - sf::Vector2f(0, diffs.a));

		eliteTri.setRotation((abs(elites.flow) == 0.f) * 90.f + (elites.flow < 0) * 180.f);
		commonTri.setRotation((abs(commoners.flow) == 0.f) * 90.f + (commoners.flow < 0) * 180.f);
		natureTri.setRotation((abs(nature.flow) == 0.f) * 90.f + (nature.flow < 0) * 180.f);
		wealthTri.setRotation((abs(wealth.flow) == 0.f) * 90.f + (wealth.flow < 0) * 180.f);

		// RENDER
		window.clear(sf::Color(0xE1E1E1FF));
		window.draw(eliteCol);
		window.draw(commonerCol);
		window.draw(natureCol);
		window.draw(wealthCol);

		window.draw(eliteTri);
		window.draw(commonTri);
		window.draw(natureTri);
		window.draw(wealthTri);

		std::printf("Flows: %f %f %f %f\n", elites.flow, commoners.flow, nature.flow, wealth.flow);

		window.display();

		frames_passed = (frames_passed + 1) * !move_on;
		frame = (frame + 1) * !(frame == 359);
		timestep++;
	}

	return 0;
}

int main() {
	std::cout << "Hello world" << std::endl;
	//return runSim(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
	renderLoop(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
}