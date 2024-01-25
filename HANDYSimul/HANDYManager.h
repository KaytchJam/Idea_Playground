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

#define BUFFER_SIZE 100
#define COLUMN_MAX_HEIGHT 300

class HANDYManager {
private:
	uint8_t eq_state;
	ObjectGroup<ColumnShape>* col_group;
	ObjectGroup<Plotter>* plot_group;
	ObjectGroup<sf::CircleShape>* tri_group;
	RingBuffer<lalg::vec4> data_stream;
	population* pops[4] = { NULL, NULL, NULL, NULL }; // {ELITES, COMMONERS, NATURE, WEALTH}

	// Initializes our 4 main populations structs
	static void init_populations(population* (&pops)[4]) {
		// struct construction
		struct human_pop* elites = new human_pop();
		struct human_pop* commons = new human_pop();
		struct nature_stock* nature = new nature_stock();
		struct wealth_stock* wealth = new wealth_stock();

		// initialize each 'stock'
		init_human_pop(*elites, BASE_ELITE_POP, BASE_ELITE_BIRTH_RATE, BASE_INEQUALITY_FACTOR, BASE_TWPC, BASE_SSPC);
		init_human_pop(*commons, BASE_COMMONER_POP, BASE_COMMONER_BIRTH_RATE, 1.f, BASE_TWPC, BASE_SSPC);
		init_nature(*nature, BASE_NATURE_STOCK, BASE_REGENERATION_RATE, BASE_DEPLETION_FACTOR, BASE_NATURE_CARRY_CAPACITY);
		init_wealth(*wealth, BASE_WEALTH_STOCK, 0.0f);

		// populating 'pop' array
		pops[0] = elites;
		pops[1] = commons;
		pops[2] = nature;
		pops[3] = wealth;
	}

	static void init_columns(ObjectGroup<ColumnShape>* col_g) {
		col_g = NULL;
	}

	static void init_plotter(ObjectGroup<Plotter>* plot_g) {
		plot_g = NULL;
	}

	static void init_tris(ObjectGroup<sf::CircleShape>* tri_g) {
		tri_g = NULL;
	}


	static void update_columns() {
		std::cout << "update_columns() : Does nothing atm" << std::endl;
	}

	static void update_plotter() {
		std::cout << "update_plotter() : Does nothing atm" << std::endl;
	}

	static void update_tris() {
		std::cout << "update_tris() : Does nothing atm" << std::endl;
	}

	HANDYManager(EQUILIBRIUM_STATES state) :
		data_stream(BUFFER_SIZE), eq_state(state) {
		init_populations(pops);
		// init_columns(this->col_group);
		// init_tris(this->tri_group);
		// init_plots(this->plot_group);
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
		std::cout << "Instance destroyed" << std::endl;
	}

	// resets & updates the manager based on the current equilibrium state 'eq_state'
	HANDYManager& reset_manager() {
		setEquilibriumValues(this->eq_state);
		std::cout << "Equilibrium values updated." << std::endl;

		// clear ringbuffer
		update_columns();
		// update tris
		update_tris();
		// update plotter
		update_plotter();

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
		calcFlow(*(wealth_stock*) pops[3], nature, common, elite);
	}

	// use euler's method of integration, compute the integral of each population for the current state
	HANDYManager& compute_stocks() {

		lalg::vec4 pop_vec = lalg::zeroVec();
		for (size_t p = 0; p < 4; p++) {
			lalg::set_index(pop_vec, (uint8_t) p, calcStock(*this->pops[p], 0, 1, true).stock); // autonomous differential equation
		}

		this->data_stream.insert(pop_vec);
		return *this;
	}

	// close the singleton
	bool close_instance() {
		delete manager_instance;
		return true;
	}

private:
	static HANDYManager* manager_instance; // for cool singleton stuff
};

HANDYManager* HANDYManager::manager_instance = nullptr;