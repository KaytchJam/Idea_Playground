#include <SFML/Graphics.hpp>

#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <fstream>

#include "MyTools/MainHelper.h"
#include "HANDYManager.h"

const double M_PI = 3.14159265358979323846;
const uint32_t ALMOST_WHITE_BUT_COOLER = 0xE1E1E1FF;
const uint32_t ELITE_COLOR = 0xBA0000FF;
const uint32_t COMMONER_COLOR = 0x000088FF;
const uint32_t NATURE_COLOR = 0x008800FF;
const uint32_t WEALTH_COLOR = 0xDF8800FF;
const char* HEADER_TEXT_FONT = "fonts/BebasNeue-Regular.ttf";

int renderLoop(const std::string STATE_NAME, float elite_total_pop, float commoner_total_pop, float nature_total_stock, float wealth_total_stock) {

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

	PointerVector<ColumnShape> colGroup({ &eliteCol, &commonerCol, &natureCol, &wealthCol });
	PointerVector<sf::CircleShape> triGroup({ &eliteTri, &commonTri, &natureTri, &wealthTri });
	{
		const float COLUMN_RADIUS = 40.f;
		const float COLUMN_HEIGHT = 100.f;
		const float COLUMN_SPACING = COLUMN_RADIUS * 4;
		unsigned int COLUMN_COLORS[4] = { ELITE_COLOR, COMMONER_COLOR, NATURE_COLOR, WEALTH_COLOR };

		sf::Vector2f REFERENCE_POSITION = mid + sf::Vector2f(COLUMN_RADIUS / 2, -1 * COLUMN_HEIGHT);
		init_columns(colGroup.cast_inner<sf::Drawable>(), COLUMN_RADIUS, COLUMN_HEIGHT, COLUMN_SPACING, COLUMN_COLORS, REFERENCE_POSITION);
		init_tris(triGroup.cast_inner<sf::Drawable>(), colGroup.cast_inner<sf::Drawable>(), COLUMN_RADIUS, COLUMN_SPACING * 3.f / 4.f);
	}

	SubCanvas column_canvas({ WIN_LENGTH / 2, WIN_HEIGHT * 4 / 5 }, { WIN_LENGTH, WIN_HEIGHT });
	column_canvas
		.link_group(&colGroup.cast_inner<sf::Drawable>())
		.link_group(&triGroup.cast_inner<sf::Drawable>())
		.setPosition({ 80.f, 100.f })
		.setBackgroundColor(sf::Color(0xFFFFFFFF))
		.set_shadow(true)
		.set_outline_color(sf::Color::Black)
		.set_outline_thickness(5.f);

	// GRAPHING
	const int RINGBUFFER_SIZE = 100;
	lalg::vec4 current_stocks;
	RingBuffer<lalg::vec4> rb(RINGBUFFER_SIZE);

	sf::Vector2f PLOT_SIZE(400, 200);
	sf::Color background_color = sf::Color::White;
	Plotter graph(PLOT_SIZE, RINGBUFFER_SIZE, sf::Color(0xBA0000FF), background_color);
	Plotter nature_graph(PLOT_SIZE, RINGBUFFER_SIZE, sf::Color(0x008800FF), background_color);
	Plotter commoner_graph(PLOT_SIZE, RINGBUFFER_SIZE, sf::Color(0x0000FCFF), background_color);
	Plotter wealth_graph(PLOT_SIZE, RINGBUFFER_SIZE, sf::Color(0xDF8800FF), background_color);

	PointerVector<Plotter> plotGroup({ &graph, &commoner_graph, &nature_graph, &wealth_graph });
	Plotter::format_window(plotGroup.cast_inner<sf::Drawable>(), mid - PLOT_SIZE, 20, 20, 2);

	SubCanvas plot_canvas({ WIN_LENGTH / 3, WIN_HEIGHT * 4 / 5 }, { WIN_LENGTH, WIN_HEIGHT });
	plot_canvas
		.link_group(&plotGroup.cast_inner<sf::Drawable>())
		.setPosition(column_canvas.getOrigin() + sf::Vector2f(column_canvas.getDimensions().x + 50.f, 0))
		.setBackgroundColor(sf::Color(0xFFFFFFFF))
		.set_shadow(true)
		.set_outline_color(sf::Color::Black)
		.set_outline_thickness(5.f);

	// column height interpolation + computation rate
	lalg::vec4 stockHeights;
	lalg::vec4 max_vector;
	const float MAX_HEIGHT = 200;
	{
		const float temp_max_human_pop = commoner_total_pop * 10;
		max_vector = { temp_max_human_pop, temp_max_human_pop, nature.capacity, std::max(wealth_total_stock * 5, temp_max_human_pop / 2)};
		stockHeights = MAX_HEIGHT * normalizeL({ elites.stock, commoners.stock, nature.stock, wealth.stock }, max_vector);
	}

	// FONT & TEXT
	sf::Font font;
	if (!font.loadFromFile(HEADER_TEXT_FONT)) {
		std::printf("An error occured. Was unable to load font %s into project.", HEADER_TEXT_FONT);
		return -1;
	}

	sf::Text text;
	text.setFont(font);
	text.setString(STATE_NAME);
	text.setCharacterSize(48);
	text.setFillColor(sf::Color::Black);
	text.setPosition({ 50, 20 });

	// run the program as long as the window is open
	unsigned int timestep = 0;
	window.setFramerateLimit(30);
	while (window.isOpen()) {
		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
		//sf::Vector2f bound_mouse_pos = bind_mouse_position(mouse_pos, WIN_LENGTH, WIN_HEIGHT);
		sf::Vector2f force = get_global_force(mouse_pos, WIN_LENGTH, WIN_HEIGHT, 10);
	
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event)) {
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::Left) {
					std::printf("<\n");
				}

				if (event.key.code == sf::Keyboard::Right) {
					std::printf(">\n");
				}
			}
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

		// FINAL UPDATES & RENDERING
		window.clear(sf::Color(ALMOST_WHITE_BUT_COOLER));
		for (int i = 0; i < colGroup.size(); i++) {
			ColumnShape* curCol = colGroup.get(i);
			float max_value = lalg::get_value(max_vector, i);

			curCol->setHeight(lalg::get_value(stockHeights, i));
			curCol->setPosition(curCol->getPosition() - sf::Vector2f(0, lalg::get_value(diffs, i)));
			triGroup.get(i)->setRotation((abs(populist[i]->flow) == 0.f) * 90.f + (populist[i]->flow < 0) * 180.f);
			plotGroup.get(i)->setVertices(rb, i, {(float)(RINGBUFFER_SIZE << 2), (float)(RINGBUFFER_SIZE << 2), max_value, max_value});
		}

		column_canvas.pull(force /*get_relative_force(bound_mouse_pos, column_canvas, 5)*/);
		plot_canvas.pull(force /*get_relative_force(bound_mouse_pos, plot_canvas, 5)*/);

		window.draw(column_canvas);
		window.draw(plot_canvas);

		window.draw(text);
		window.display();
		timestep++;
	}

	return 0;
}

// refactoring?

int HANDY_test() {
	const int WIN_LENGTH = 1280;
	const int WIN_HEIGHT = 720;
	sf::RenderWindow window(sf::VideoMode(WIN_LENGTH, WIN_HEIGHT), "Human and Nature Dynamics: Modelling Differential...");
	HANDYManager& man = HANDYManager::get_instance();

	window.setFramerateLimit(30);
	while (window.isOpen()) {
		sf::Event event;
		sf::Vector2f force = get_global_force(sf::Mouse::getPosition(window), window.getSize().x, window.getSize().y, 10);

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			man.handle_events(event);
		}

		// rendering
		window.clear(sf::Color(ALMOST_WHITE_BUT_COOLER));
		window.draw(man.compute_flows().compute_stocks().update_drawables().get_canvas().pull(force));
		window.draw(man.get_eq_text());
		window.display();
	}

	return man.print_state().close_instance();
}

int main() {
	std::cout << "Hello world" << std::endl;
	HANDY_test();

	//EQUILIBRIUM_STATES HANDY_STATE = EQUITABLE_CYCLES_AND_REVIVAL;
	//setEquilibriumValues(HANDY_STATE);
	//renderLoop(EQUILIBRIUM_STATE_NAMES[HANDY_STATE], BASE_ELITE_POP, BASE_COMMONER_POP, BASE_NATURE_STOCK, BASE_WEALTH_STOCK);
}

//int pseudo_main() {
//	std::unique_ptr<HANDYManager> h_man(initialize_instance()); 
//	// inside initialize instance ->
//		// Array of populations
//		// initialize columns -> insert into PointerVector
//		// initialize tris -> insert into PointerVector
//		// initialize plots -> insert into PointerVector
//
//	// render loop
//	while (window) {
//		pre_events();
//		events();
//		update();
//		render();
//		post_render();
//	}
//
//	return 0;
//}
