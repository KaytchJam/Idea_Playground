#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "MyTools/MainHelper.h"

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
		bool cond = local_max < rb.peek();
		local_max = rb.peek() * cond + local_max * !cond;

		float prev = rb.front();
		size_t index = 0;
		RingBuffer<float>::iterator it = rb.begin();
		while (it != rb.end()) {
			std::printf("IT: %d, END: %x\n", it.m_index, rb.end().m_index);
			std::printf("Index: %d\n", index);

			float prev_normalized = normToRange(prev, { LOWER, local_max }, { 0, graph_bounds.getSize().y });
			float cur_normalized = normToRange(it.m_data, { LOWER, local_max }, { 0, graph_bounds.getSize().y });
			float index_normalized = normToRange((index - 2.f) * (index != 0), { 0.f, (float)vertices.size()/*(float)rb.size() - rb.capacity()*/ }, { 0.f, (float)graph_bounds.getSize().x });
			float index_plus_one_normalized = normToRange((float)(index), { 0.f, (float)vertices.size()/*(float)rb.size() - rb.capacity()*/ }, { 0.f, (float)graph_bounds.getSize().x });

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
		window.draw(&vertices[0], (size_t) (rb.size() - rb.capacity()) * 2, sf::Lines);
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
		p.setVertices(rb, 0, { (float)(rb.size() << 1), (float)(rb.size() << 1), local_max, local_max }, { 0, 0, LOWER, LOWER });

		window.clear(sf::Color(0xFFFFFFFF));
		window.draw(p);
		window.display();

		frame = (frame + 1) * !(frame == 100);
		timestep++;
	}
	return 0;
}

int runSimToFile(float elite_total_pop, float commoner_total_pop, float nature_total_stock, float wealth_total_stock) {
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

uint32_t ALMOST_WHITE_BUT_COOLER = 0xE1E1E1FF;
int text_test(const std::string& sentence, const std::string& font_file_path) {
	sf::Font font;
	if (!font.loadFromFile(font_file_path)) {
		std::printf("An error occured. Was unable to load font %s into project.", font_file_path.c_str());
		return -1;
	}

	sf::Text text;
	text.setFont(font);
	text.setString(sentence);
	text.setCharacterSize(48);
	text.setFillColor(sf::Color::Black);

	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);
	text.setPosition(mid - sf::Vector2f(text.getGlobalBounds().getSize().x / 2, text.getGlobalBounds().getSize().y / 2));

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(ALMOST_WHITE_BUT_COOLER));
		window.draw(text);
		window.display();
	}

	return 0;
}

int subcanvas_test() {
	// WINDOW STATES
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "");
	const sf::Vector2f mid(WIN_LENGTH / 2, WIN_HEIGHT / 2);

	// OBJECTS & ABSTRACTIONS FOR RENDERING
	sf::RectangleShape t_recs({ 300, 300 });
	t_recs.setPosition(mid - sf::Vector2f(150, 150));
	t_recs.setFillColor(sf::Color(0x0000AAFF));
	ObjectGroup<sf::Drawable> rect_group({ &t_recs });
	// Add RectangleShape to ObjectGroup so I can bind it to our canvases ^^^

	SubCanvas canva({ WIN_LENGTH / 3, WIN_HEIGHT / 3 }, { WIN_LENGTH, WIN_HEIGHT });
	canva.setPosition({ 300, 300 });

	SubCanvas canva2({ WIN_LENGTH / 2, WIN_HEIGHT / 2 }, { WIN_LENGTH, WIN_HEIGHT });
	canva2.setPosition({ WIN_LENGTH / 2 - 100.f, 50.f }).setBackgroundColor(sf::Color::Cyan);

	// """SHADOWS"""
	sf::RectangleShape canva_shadow(canva.getDimensions());
	canva_shadow.setPosition(canva.getOrigin());
	canva_shadow.setFillColor(sf::Color(0x00000055));
	sf::RectangleShape canva2_shadow(canva2.getDimensions());
	canva2_shadow.setPosition(canva2.getOrigin());
	canva2_shadow.setFillColor(sf::Color(0x00000055));

	// MOUSE NORMALIZATION & RANGE (-1, 1) BINDING
	const lalg::mat4 bound_mouse_coords =
	{{ 2.f / WIN_LENGTH, 0.f, -1.f,   0.f},
	 { 0.f, 2.f / WIN_HEIGHT,  -1.f,  0.f},
	 { 0.f, 0.f,                1.f,  0.f},
	 { 0.f, 0.f,                0.f,  1.f} };

	// STATES
	bool already_linked = false;
	const float SHIFT_VALUE = 5;

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		sf::Event event;
		//sf::Vector2f normalized_pos;
		sf::Vector2f force1;
		sf::Vector2f force2;
		{
			sf::Vector2i local_pos = sf::Mouse::getPosition(window);
			int call = 0;
			sf::Vector2f bound_pos = lalgToSf(lalg::map_capture(sfToLalg({ (float)local_pos.x, (float)local_pos.y }), [&WIN_LENGTH, &WIN_HEIGHT, &call](float f) {
				return std::fmaxf(0.f, std::fminf(f, ((float)WIN_LENGTH * !(call == 1.f)) + ((float)WIN_HEIGHT * (call++ == 1.f)))); }));
			force1 = get_relative_force(bound_pos, canva, 10);
			force2 = get_relative_force(bound_pos, canva2, 10);
		}

		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed) {
				if (!already_linked && event.key.code == sf::Keyboard::Space) {
					canva.link_group(&rect_group);
					canva2.link_group(&rect_group);
					already_linked = true;
					std::printf("Linked.\n");
				}

				float shift_x = SHIFT_VALUE * (event.key.code == sf::Keyboard::Right) - SHIFT_VALUE * (event.key.code == sf::Keyboard::Left);
				float shift_y = SHIFT_VALUE * (event.key.code == sf::Keyboard::Down) - SHIFT_VALUE * (event.key.code == sf::Keyboard::Up);
				t_recs.move({ shift_x, shift_y });
			}
		}

		// pull effect
		//canva.pull({ normalized_pos.x * 10, normalized_pos.y * 10});
		//canva2.pull({ normalized_pos.x * 20, normalized_pos.y * 20});
		canva.pull(force1);
		canva2.pull(force2);

		// rendering
		window.clear(sf::Color(ALMOST_WHITE_BUT_COOLER));
		window.draw(t_recs);
		window.draw(canva_shadow);
		window.draw(canva);
		window.draw(canva2_shadow);
		window.draw(canva2);
		window.display();
	}

	return 0;
}