#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "MyTools/MainHelper.h"

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

//static lalg::vec4 normalizeL(const lalg::vec4& vals, const lalg::vec4& maxes) {
//	return lalg::map(lalg::diag(maxes), [](float f) { return 1 / (f + (f == 0.0f)) - (f == 0.0f); }) * vals;
//}

static lalg::vec4 normalizeL(const lalg::vec4& vals, const lalg::vec4& maxes) {
	return lalg::diag(lalg::map(maxes, [](float f) { return 1 / (f + 1 * (f == 0.f)); })) * vals;
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

void init_columns(std::vector<ColumnShape*>& cols, float c_rad, float c_height, float c_spacing, uint32_t CCs[4], sf::Vector2f ref_pos) {
	for (int i = 0; i < cols.size(); i++) {
		ColumnShape* col_ptr = cols[i];
		col_ptr->setRadius(c_rad); // column radius
		col_ptr->setHeight(c_height);
		col_ptr->setPosition(ref_pos + sf::Vector2f((-2 + i) * c_spacing, 0));
		col_ptr->setColor(CCs[i]);
	}
}

void init_tris(std::vector<sf::CircleShape*>& tris, std::vector<ColumnShape*> cols, float rad, float spacing) {
	for (int i = 0; i < tris.size(); i++) {
		sf::CircleShape* tri = tris[i];
		ColumnShape* curCol = cols[i];
		tri->setRadius(rad);
		tri->setPointCount(3);
		tri->setOrigin({ rad,rad });
		tri->move(curCol->getPosition() + sf::Vector2f(0.f, curCol->getHeight() + spacing) + sf::Vector2f(rad, rad));
		tri->setFillColor(curCol->getColor());
	}
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

	// COLUMNS
	ColumnShape natureCol, wealthCol, commonerCol, eliteCol;
	sf::CircleShape eliteTri, commonTri, natureTri, wealthTri;
	std::vector<ColumnShape*> colList({ &eliteCol, &commonerCol, &natureCol, &wealthCol });
	std::vector<sf::CircleShape*> triList({ &eliteTri, &commonTri, &natureTri, &wealthTri });
	{
		const float COLUMN_RADIUS = 20.f;
		const float COLUMN_HEIGHT = 80.f;
		const float COLUMN_SPACING = 60.f;
		unsigned int COLUMN_COLORS[4] = { 0xBA0000FF, 0x000088FF, 0x008800FF, 0xDF8800FF };
		sf::Vector2f REFERENCE_POSITION = mid - sf::Vector2f(COLUMN_RADIUS / 2, COLUMN_HEIGHT);

		init_columns(colList, COLUMN_RADIUS, COLUMN_HEIGHT, COLUMN_SPACING, COLUMN_COLORS, REFERENCE_POSITION);
		init_tris(triList, colList, COLUMN_RADIUS, COLUMN_SPACING);
	}

	// GRAPHING
	int RINGBUFFER_SIZE = 100;
	lalg::vec4 current_stocks;
	RingBuffer<lalg::vec4> rb(RINGBUFFER_SIZE);

	Plotter graph(sf::Vector2f(200, 200), 200, sf::Color(0xBA0000FF), sf::Color::Black);
	graph.setPosition(sf::Vector2f(WIN_LENGTH - 250, 50));

	Plotter nature_graph(sf::Vector2f(200, 200), 200, sf::Color(0x008800FF), sf::Color::Black);
	nature_graph.setPosition(graph.getPosition() + sf::Vector2f(0, 220));

	Plotter commoner_graph(sf::Vector2f(200, 200), 200, sf::Color(0x0000FCFF), sf::Color::Black);
	commoner_graph.setPosition(graph.getPosition() - sf::Vector2f(220, 0));

	Plotter wealth_graph(sf::Vector2f(200, 200), 200, sf::Color(0xDF8800FF), sf::Color::Black);
	wealth_graph.setPosition(nature_graph.getPosition() - sf::Vector2f(220, 0));

	ObjectGroup<Plotter> plotGroup;
	plotGroup.add(&graph).add(&commoner_graph).add(&nature_graph).add(&wealth_graph);

	std::vector<Plotter*> plots({ &graph, &commoner_graph, &nature_graph, &wealth_graph });

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
	lalg::vec4 stockHeights = MAX_HEIGHT * normalizeL({ elites.stock, commoners.stock, nature.stock, wealth.stock }, max_vector);
	//lalg::printVec(stockHeights);
	//lalg::printVec(max_vector);
	//stockHeights = normalizeL(stockHeights, max_vector) * MAX_HEIGHT;

	// run the program as long as the window is open
	window.setFramerateLimit(30);
	while (window.isOpen()) {
		std::printf("timestep %d: ", timestep);
		//printVec(stockHeights);

		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		calcAllFlows(elites, commoners, nature, wealth); // computing dS/dt
		current_stocks = calcAllStocks(elites, commoners, nature, wealth); // compute S -> return vector of all
		rb.insert(current_stocks);

		update_maxes(max_vector, current_stocks);
		lalg::vec4 diffs;
		{
			lalg::vec4 normed_stocks = normalizeL(current_stocks, max_vector);
			stockHeights = MAX_HEIGHT * normed_stocks;
			diffs = stockHeights - lalg::vec4({ eliteCol.getHeight(), commonerCol.getHeight(), natureCol.getHeight(), wealthCol.getHeight() });
		}

		window.clear(sf::Color(0xE1E1E1FF));
		for (int i = 0; i < colList.size(); i++) {
			ColumnShape* curCol = colList[i];
			sf::CircleShape* curTri = triList[i];
			Plotter* curPlot = plots[i];
			float max_value = getValue(max_vector, i);

			curCol->setHeight(lalg::getValue(stockHeights, i));
			curCol->setPosition(curCol->getPosition() - sf::Vector2f(0, lalg::getValue(diffs, i)));
			curTri->setRotation((abs(wealth.flow) == 0.f) * 90.f + (populist[i]->flow < 0) * 180.f);
			curPlot->setVertices(rb, i, { (float)(RINGBUFFER_SIZE << 1), (float)(RINGBUFFER_SIZE << 1), max_value, max_value});

			window.draw(*curCol);
			window.draw(*curTri);
			window.draw(*curPlot);
		}

		window.display();
		bool move_on = frames_passed == (FRAME_DELAY - 1);
		frames_passed = (frames_passed + 1) * !move_on;
		frame = (frame + 1) * !(frame == 359);
		timestep++;
		std::printf("\n");
	}

	return 0;
}

int graphTest() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "Curve / Graph Testing");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);

	float frame = 1;
	const unsigned int BUFFER_SIZE = 100;
	RingBuffer<float> rb(BUFFER_SIZE);

	sf::Color PLOT_COLOR = sf::Color::Red;

	sf::RectangleShape graph_bounds({ 500.f, 500.f });
	graph_bounds.setFillColor(sf::Color::Black);
	//graph_bounds.setFillColor(sf::Color(200, 200, 200, 255));
	graph_bounds.setPosition(mid - sf::Vector2f(graph_bounds.getSize().x / 2, graph_bounds.getSize().y / 2));

	float timestep = 0;
	float local_max = 10.f;
	float LOWER = -10.f;

	std::vector<sf::Vertex> vertices(BUFFER_SIZE * 2);
	rb.insert(0.f);

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// add frame # to ringbuffer
		rb.insert(2 * sin(3 / (1 + frame)) + 3 * cos(5 * frame) + 4 * sin(6 * frame) + cos(3 / (1 + frame)));
		bool cond = local_max< rb.peek();
		local_max = rb.peek() * cond + local_max * !cond;

		float prev = rb.front();
		int index = 0;
		RingBuffer<float>::iterator it = rb.begin();
		while (it != rb.end()) {
			std::printf("IT: %d, END: %d\n", it.m_index, rb.end().m_index);
			std::printf("Index: %d\n", index);

			float prev_normalized = normToRange(prev, { LOWER, local_max }, { 0, graph_bounds.getSize().y } );
			float cur_normalized = normToRange(it.m_data, { LOWER, local_max }, { 0, graph_bounds.getSize().y });
			float index_normalized = normToRange((index - 2.f) * (index != 0), {0.f, (float)vertices.size()/*(float)rb.size() - rb.capacity()*/}, {0.f, (float)graph_bounds.getSize().x});
			float index_plus_one_normalized = normToRange((float) (index), { 0.f, (float)vertices.size()/*(float)rb.size() - rb.capacity()*/}, {0.f, (float)graph_bounds.getSize().x});

			std::printf("(%f, %f) -> (%f, %f)\n\n", index_normalized, prev_normalized, index_plus_one_normalized, cur_normalized);

			vertices[index].position = graph_bounds.getPosition() + sf::Vector2f(index_normalized, graph_bounds.getSize().y - prev_normalized);
			vertices[index].color = PLOT_COLOR;
			vertices[index + 1].position = graph_bounds.getPosition() + sf::Vector2f(index_plus_one_normalized, graph_bounds.getSize().y - cur_normalized);
			vertices[index + 1].color = PLOT_COLOR;

			prev = it.m_data;
			it++;
			index += 2;
		}

		window.clear(sf::Color(0xFFFFFFFF));
		window.draw(graph_bounds);
		window.draw(&vertices[0], (rb.size() - rb.capacity()) * 2, sf::Lines);
		window.display();
		frame = (frame + 1) * !(frame == 359);
		timestep++;
	}

	return 0;
}

static void bufferTest() {

	RingBuffer<float> rb(101);
	for (int i = 0; i < 100; i++) {
		rb.insert((float)i);
	}

	rb.stream(NULL, [](float f, void* cb) {
			std::printf("%f\n", f);
	});

	auto it = rb.begin();
	while (it != rb.end()) {
		std::printf("%f\n", it.getData());
		it++;
	}
}

static int plotterTest() {

	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "Curve / Graph Testing");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);

	float frame = 1;
	const unsigned int BUFFER_SIZE = 100;

	RingBuffer<lalg::vec4> rb(BUFFER_SIZE);
	sf::Color PLOT_COLOR = sf::Color::Red;

	Plotter p({ 500.f, 500.f }, BUFFER_SIZE);
	p.setColor(PLOT_COLOR);
	p.setPosition(mid - sf::Vector2f(p.getSize().x / 2, p.getSize().y / 2));
	p.setBackgroundColor(sf::Color::Black);

	float timestep = 0;
	float local_max = 10.f;
	float LOWER = -10.f;

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		bool cond = local_max < rb.peek().r;
		local_max = rb.peek().r * cond + local_max * !cond;

		// add frame # to ringbuffer
		rb.insert({ 2 * sin(3 / (1 + frame)) + 3 * cos(5 * frame) + 4 * sin(6 * frame) + cos(3 / (1 + frame)), 0, 0, 0 });
		p.setVertices(rb, 0, { (float)(rb.size() << 1), (float) (rb.size() << 1), local_max, local_max}, {0, 0, LOWER, LOWER});

		window.clear(sf::Color(0xFFFFFFFF));
		window.draw(p);
		window.display();

		frame = (frame + 1) * !(frame == 100);
		timestep++;
	}
	return 0;
}

int main() {
	std::cout << "Hello world" << std::endl;
	//return runSim(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
	setEquilibriumValues(EGALITARIAN_CYCLES_AND_REVIVAL);
	renderLoop(BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
	//bufferTest();
	//plotterTest();
}