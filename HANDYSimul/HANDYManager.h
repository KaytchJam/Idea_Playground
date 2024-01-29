#pragma once

// standard library
#include <iostream>

// HANDY Equations & Equilibrium states
#include "Populations.h"

// Data Structures & Linear Algebra
#include "MyTools/ObjectGroup.h"
#include "MyTools/RingBuffer.h"
#include "MyTools/MyV.h"

// Graphics components
#include "CustomShapes/ColumnShape.h"
#include "CustomShapes/Plotter.h"

// RINGBUFFER
#define BUFFER_SIZE 100

// COLUMNS
#define COL_MAX_HEIGHT 200.0f
#define COL_BASE_HEIGHT 100.f
#define COL_RAD 20.0f
#define COL_SPACING_FACTOR 2
#define COL_TYPE ColumnShape::ColumnType::BAR
#define COL_REF_POS sf::Vector2f(640.f, 360.f)

// PLOTS
#define PLT_LENGTH 200.f
#define PLT_HEIGHT 400.f

// CANVAS
#define CANV_LENGTH 960
#define CANV_HEIGHT 540

// POPULATION COLORS
#define E_COLOR 0xBA0000FF
#define C_COLOR 0x000088FF
#define N_COLOR 0x008800FF
#define W_COLOR 0xDF8800FF

class HANDYManager {
private:
	// states & data
	uint8_t eq_state;
	population* pops[4] = { nullptr, nullptr, nullptr, nullptr }; // {ELITES, COMMONERS, NATURE, WEALTH}

	lalg::vec4 pop_stock_maxes;
	RingBuffer<lalg::vec4> data_stream;

	//sf::Text txt;
	//sf::Font fnt;

	// Object Groups
	ObjectGroup<ColumnShape> col_group;
	ObjectGroup<Plotter> plot_group;
	ObjectGroup<sf::CircleShape> tri_group;

	// main canvas
	SubCanvas main_canvas;

	// update the values in each stock struct based on the current equilibrium values
	static void set_population_values(human_pop& ep, human_pop& cp, nature_stock& ns, wealth_stock& ws) {
		init_human_pop(ep, BASE_ELITE_POP, BASE_ELITE_BIRTH_RATE, BASE_INEQUALITY_FACTOR, BASE_TWPC, BASE_SSPC);
		init_human_pop(cp, BASE_COMMONER_POP, BASE_COMMONER_BIRTH_RATE, 1.f, BASE_TWPC, BASE_SSPC);
		init_nature(ns, BASE_NATURE_STOCK, BASE_REGENERATION_RATE, BASE_DEPLETION_FACTOR, BASE_NATURE_CARRY_CAPACITY);
		init_wealth(ws, BASE_WEALTH_STOCK, 0.0f);
	}

	/*sf::Font font;
	if (!font.loadFromFile(HEADER_TEXT_FONT)) {
		std::printf("An error occured. Was unable to load font %s into project.", HEADER_TEXT_FONT);
		return -1;
	}*/

	// Initializes our 4 main populations structs
	static void init_populations(population* (&pops)[4]) {
		// struct construction
		struct human_pop* elites = new human_pop();
		struct human_pop* commons = new human_pop();
		struct nature_stock* nature = new nature_stock();
		struct wealth_stock* wealth = new wealth_stock();

		// initialize each 'stock'
		set_population_values(*elites, *commons, *nature, *wealth);

		// populating 'pop' array
		pops[0] = elites;
		pops[1] = commons;
		pops[2] = nature;
		pops[3] = wealth;
	}

	// Initialize columns for ObjectGroup
	static void init_columns(ObjectGroup<ColumnShape>& col_g) {
		ColumnShape* ep_col = new ColumnShape();
		ColumnShape* cp_col = new ColumnShape();
		ColumnShape* ns_col = new ColumnShape();
		ColumnShape* ws_col = new ColumnShape();

		uint8_t index = 0;
		const uint32_t colors[4] = { E_COLOR, C_COLOR, N_COLOR, W_COLOR };
		const sf::Vector2f TRUE_REF_POS = COL_REF_POS - sf::Vector2f(COL_RAD * 2 * COL_SPACING_FACTOR * 2 - COL_RAD, COL_BASE_HEIGHT); //- sf::Vector2f(COL_RAD * 2, 100.f);

		col_g.add_all({ ep_col, cp_col, ns_col, ws_col }).map_capture([&colors, &index, &TRUE_REF_POS](ColumnShape* col) {
			col->setRadius(COL_RAD)
				.setPosition(TRUE_REF_POS + sf::Vector2f((index) * COL_RAD * 2 * COL_SPACING_FACTOR, 0.f))
				.setHeight(COL_BASE_HEIGHT)
				.set_column_type(ColumnShape::ColumnType::BAR)
				.setColor(colors[index]);
			index++;
		});
	}

	// TODO
	static void init_plotter(ObjectGroup<Plotter>& plot_g) {
		const uint32_t colors[4] = { E_COLOR, C_COLOR, N_COLOR, W_COLOR };
		for (uint8_t index = 0; index < 4; index++) {
			Plotter* plt = new Plotter({ PLT_LENGTH, PLT_HEIGHT }, BUFFER_SIZE, sf::Color(colors[index]), sf::Color(0xFFFFFFFF));
			plot_g.add(plt);
		}
	}

	// TODO
	static void init_tris(ObjectGroup<sf::CircleShape>& tri_g) {
		// = NULL;
	}

	// TODO: should just update the heights based on the current state
	static void reset_columns(ObjectGroup<ColumnShape>& col_g) {
		uint8_t index = 0;
		const sf::Vector2f TRUE_REF_POS = COL_REF_POS - sf::Vector2f(COL_RAD * 2 * COL_SPACING_FACTOR * 2 - COL_RAD, COL_BASE_HEIGHT);
		col_g.map_capture([&TRUE_REF_POS, &index](ColumnShape* cs) {
			cs->setPosition(TRUE_REF_POS + sf::Vector2f(index * COL_RAD * 2 * COL_SPACING_FACTOR, 0.f))
				.setHeight(COL_BASE_HEIGHT);
			index++;
		});
	}

	// TODO
	static void reset_plotter() {
		// std::cout << "update_plotter() : Does nothing atm" << std::endl;
	}

	// TODO
	static void reset_tris() {
		// std::cout << "update_tris() : Does nothing atm" << std::endl;
	}

	// Constructor
	HANDYManager(EQUILIBRIUM_STATES state) :
		data_stream(BUFFER_SIZE), eq_state(state), main_canvas({ CANV_LENGTH, CANV_HEIGHT }, { 1280, 720 }) {
		init_populations(this->pops);
		init_columns(this->col_group);
		// init_tris(this->tri_group);
		// init_plots(this->plot_group);
		
		this->pop_stock_maxes = { BASE_COMMONER_POP * 10, BASE_COMMONER_POP * 10, BASE_NATURE_CARRY_CAPACITY, std::fmaxf(BASE_WEALTH_STOCK * 5, BASE_COMMONER_POP * 5) };
	}

public:
	// A singleton class, returns an instance of the HANDYManager.
	static HANDYManager& get_instance() { // singleton object -> only one instance of this class
		if (manager_instance == NULL) manager_instance = new HANDYManager(EQUILIBRIUM_STATES::EGALITARIAN_SOFT_LANDING);
		return *manager_instance;
	}

	// Destructor
	~HANDYManager() {
		for (int i = 0; i < 4; i++) delete pops[i];
		col_group.map_mut([](ColumnShape* cs) { delete cs; }).clear();
		plot_group.map_mut([](Plotter* plt) { delete plt; }).clear();
		std::cout << "Instance destroyed" << std::endl;
	}

	// resets & updates the manager based on the current equilibrium state 'eq_state'
	HANDYManager& reset_manager() {
		human_pop& ep = *(human_pop*)this->pops[0];
		human_pop& cp = *(human_pop*)this->pops[1];
		nature_stock& ns = *(nature_stock*)this->pops[2];
		wealth_stock& ws = *(wealth_stock*)this->pops[3];

		setEquilibriumValues(this->eq_state);
		set_population_values(ep, cp, ns, ws);
		this->pop_stock_maxes = { BASE_COMMONER_POP * 10, BASE_COMMONER_POP * 10, BASE_NATURE_CARRY_CAPACITY, std::fmaxf(BASE_WEALTH_STOCK * 5, BASE_COMMONER_POP * 5) };
		this->data_stream.reset();

		// clear ringbuffer
		reset_columns(col_group);
		// update tris
		reset_tris();
		// update plotter
		reset_plotter();

		return *this;
	}

	// Transition to the next equilibrium equation state
	HANDYManager& increment_state() {
		this->eq_state++;
		eq_state = eq_state * !(eq_state == EQUILIBRIUM_STATES::TOTAL_STATES);
		return reset_manager();
	}

	// Transition to the previous equilibrium equation state
	HANDYManager& decrement_state() {
		bool at_lower = eq_state == 0;
		eq_state = (eq_state - 1) * !at_lower + (EQUILIBRIUM_STATES::TOTAL_STATES - 1) * at_lower;
		return reset_manager();
	}

	// manually set the state of the manager to one of the equilibrium equations
	HANDYManager& set_state(EQUILIBRIUM_STATES state) {
		if (state >= EQUILIBRIUM_STATES::TOTAL_STATES || state < 0) std::printf("Invalid state!");

		this->eq_state = state;
		return reset_manager();
	}

	// print the current state (debugging)
	HANDYManager& print_state() {
		std::printf("%d - %s\n", this->eq_state, EQUILIBRIUM_STATE_NAMES[this->eq_state]);
		return *this;
	}

	// update each population, calculate the derivative, i.e. the 'flow' for each
	HANDYManager& compute_flows() {
		human_pop& elite = *(human_pop*) pops[0];
		human_pop& common = *(human_pop*) pops[1];
		nature_stock& nature = *(nature_stock*) pops[2];

		// computing each equation 
		calcFlow(elite); 
		calcFlow(common); 
		calcFlow(nature, common); 
		calcFlow(*(wealth_stock*) pops[3], nature, elite, common);

		return *this;
	}

	// update the current maxes according to ringbuffer data
	HANDYManager& update_maxes() {
		uint8_t index = 0;
		const lalg::vec4 current_stocks = data_stream.peek();
		lalg::map_capture(this->pop_stock_maxes, [&current_stocks, &index](float f) { return std::fmaxf(f, lalg::getValue(current_stocks, index++)); });
		return *this;
	}

	// updates the current maxes accoridng to the passed in data
	HANDYManager& update_maxes(const lalg::vec4& pop_vec) {
		this->pop_stock_maxes.r = std::fmaxf(this->pop_stock_maxes.g, (std::fmaxf(pop_vec.g, pop_vec.r)));
		this->pop_stock_maxes.g = this->pop_stock_maxes.r; //std::fmaxf(pop_vec.g, this->pop_stock_maxes.g);
		this->pop_stock_maxes.a = std::fmaxf(pop_vec.a, this->pop_stock_maxes.a);
		return *this;
	}

	// use euler's method of integration, compute the integral of each population for the current state
	HANDYManager& compute_stocks() {
		lalg::vec4 pop_vec = lalg::zeroVec();
		for (size_t p = 0; p < 4; p++) lalg::set_index(pop_vec, (uint8_t) p, calcStock(*this->pops[p], 0, 1, true).stock); // autonomous differential equation

		//pop_vec.b = fmaxf(-1.f, pop_vec.b);
		//this->pops[3]->stock = pop_vec.b;

		this->data_stream.insert(pop_vec);
		return update_maxes(pop_vec);
	}

	// updates column heights according to ringbuffer values
	HANDYManager& update_columns() {

		const lalg::vec4 nuevo_stock_heights = lalg::diag(lalg::map(this->pop_stock_maxes, [](float f) { 
			return 1.f / f; 
		})) * this->data_stream.peek() * COL_MAX_HEIGHT;

		const lalg::vec4 diffs = nuevo_stock_heights - lalg::vec4({ 
			col_group.get(0)->getHeight(), 
			col_group.get(1)->getHeight(),
			col_group.get(2)->getHeight(),
			col_group.get(3)->getHeight()
		});

		for (uint8_t index = 0; index < 4; index++) {
			ColumnShape& cur_col = *col_group.get(index);
			cur_col.setHeight(lalg::getValue(nuevo_stock_heights, index))
				.setPosition(cur_col.getPosition() - sf::Vector2f(0, lalg::getValue(diffs, index)));
		}

		return *this;
	}

	// print the current stock values
	HANDYManager& print_stocks() {
		std::printf("E: (%f) C: (%f) N: (%f) W: (%f)\n", 
			this->pops[0]->stock, 
			this->pops[1]->stock, 
			this->pops[2]->stock, 
			this->pops[3]->stock
		);
		return *this;
	}

	// print the current flow values
	HANDYManager& print_flows() {
		std::printf("E': (%f) C': (%f) N': (%f) W': (%f)\n", 
			this->pops[0]->flow, 
			this->pops[1]->flow, 
			this->pops[2]->flow, 
			this->pops[3]->flow
		);
		return *this;
	}

	// print column positions
	HANDYManager& print_col_pos() {
		std::printf("E: (%f, %f) C: (%f, %f) N: (%f, %f) W: (%f, %f)\n",
			this->col_group.get(0)->getPosition().x,
			this->col_group.get(0)->getPosition().y,
			this->col_group.get(1)->getPosition().x,
			this->col_group.get(1)->getPosition().y,
			this->col_group.get(2)->getPosition().x,
			this->col_group.get(2)->getPosition().y,
			this->col_group.get(3)->getPosition().x,
			this->col_group.get(3)->getPosition().y
		);
		return *this;
	}

	// close the singleton
	bool close_instance() {
		delete manager_instance;
		manager_instance = nullptr;
		return true;
	}

	// print column group
	ObjectGroup<ColumnShape>& get_col_group() {
		return this->col_group;
	}

private:
	static HANDYManager* manager_instance; // for cool singleton stuff
};

HANDYManager* HANDYManager::manager_instance = nullptr;