#pragma once

// standard library
#include <iostream>
#include <sstream>

// HANDY Equations & Equilibrium states
#include "Populations.h"

// Data Structures & Linear Algebra
#include "MyTools/PointerVector.h"
#include "MyTools/RingBuffer.h"
#include "MyTools/MyV.h"

// Graphics components
#include "CustomShapes/ColumnShape.h"
#include "CustomShapes/Plotter.h"

// RINGBUFFER
#define BUFFER_SIZE 100

// COLUMNS
#define COL_MAX_HEIGHT 300.0f
#define COL_BASE_HEIGHT 100.f
#define COL_RAD 20.0f
#define COL_SPACING_FACTOR 2
#define COL_TYPE ColumnShape::ColumnType::BAR
#define COL_REF_POS sf::Vector2f(320.f, 540.f)//sf::Vector2f(640.f, 360.f)

// TRIS
#define TRI_ADDITIONAL_SHIFT sf::Vector2f(0, 20.f)

// PLOTS
#define PLT_LENGTH 200.f
#define PLT_HEIGHT 200.f
#define PLT_FORMAT_REF_POS sf::Vector2f(640.f, 160.f)
#define PLT_FORMAT_V_SPACING 20
#define PLT_FORMAT_H_SPACING 20
#define PLT_FORMAT_ROWS 2
#define PLT_BACKGROUND_COLOR 0x9B959BFF
#define PLT_ADDITIONAL_SHIFT sf::Vector2f(100.f, 0)

// CANVAS
#define CANV_LENGTH 960
#define CANV_HEIGHT 540
#define CANV_REF_POS sf::Vector2f(640.f, 400.f)
#define CANV_BACKGROUND_COLOR 0xFFFFFFFF
#define CANV_OUTLINE_COLOR 0x000000FF

// TEXT
#define TEXT_REF_POS sf::Vector2f(130.f, 75.f)
#define FONT_PATH_EQUILIBRIUM  "fonts/BebasNeue-Regular.ttf"
#define POP_TEXT_COLOR 0xFFFFFFFF

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

	// Font / Text
	sf::Text* eq_txt;
	sf::Font* eq_fnt;

	// Pointer Vectors
	PointerVector<ColumnShape> col_group;
	PointerVector<Plotter> plot_group;
	PointerVector<sf::CircleShape> tri_group;
	PointerVector<sf::Text> pop_text_group;
	PointerVector<sf::Text> col_text_group;

	// main canvas
	SubCanvas* main_canvas;

	//static ColumnShape* to_col(sf::Drawable* d) { return (ColumnShape*)d; }
	//static Plotter* to_plot(sf::Drawable* d) { return (Plotter*)d; }
	//static sf::CircleShape* to_tri(sf::Drawable* d) { return (sf::CircleShape*)d; }

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

	// initializes all our drawables in 1 iteration
	static void init_drawables(PointerVector<ColumnShape>& col_g, PointerVector<sf::CircleShape>& tri_g,
		PointerVector<Plotter>& plt_g, PointerVector<sf::Text>& pop_text_g, PointerVector<sf::Text>& col_text_g, sf::Font& font) {

		// params
		const uint32_t colors[4] = { E_COLOR, C_COLOR, N_COLOR, W_COLOR };
		const sf::Vector2f TRUE_REF_POS = COL_REF_POS - sf::Vector2f(COL_RAD * 2 * COL_SPACING_FACTOR * 2 - COL_RAD, COL_BASE_HEIGHT + COL_RAD * 3);
		const float TRI_SPACING = COL_RAD * COL_SPACING_FACTOR * 3.f / 4.f;
		const sf::Vector2f RAD_VECTOR = sf::Vector2f(COL_RAD, COL_RAD);
		const char* POP_NAMES[4] = {"ELITES", "COMMONERS", "NATURE", "WEALTH"};

		for (uint8_t index = 0; index < 4; index++) {
			// column init
			ColumnShape* col = col_g.add((new ColumnShape())
				->setRadius(COL_RAD)
				.setPosition(TRUE_REF_POS + sf::Vector2f((index)*COL_RAD * 2 * COL_SPACING_FACTOR, 0.f))
				.setHeight(COL_BASE_HEIGHT)
				.set_column_type(ColumnShape::ColumnType::BAR)
				.setColor(colors[index])
			).set_ownership(true).get(index);

			// pop text
			sf::Text& pop_text = *pop_text_g.add(new sf::Text()).set_ownership(true).get(index);
			pop_text.setFont(font);
			pop_text.setString(POP_NAMES[index]);
			pop_text.setCharacterSize(48);
			pop_text.setFillColor(sf::Color(POP_TEXT_COLOR));
			pop_text.setPosition(col->getPosition() + sf::Vector2f(0.f, col->getHeight() + COL_RAD));
			pop_text.rotate(-90.f);

			//  col text
			sf::Text& col_text = *col_text_g.add(new sf::Text()).set_ownership(true).get(index);
			col_text.setFont(font);
			col_text.setString("0");
			col_text.setCharacterSize(24);
			col_text.setPosition(col->getPosition() - sf::Vector2f(col_text.getLocalBounds().getSize().x / 2, COL_RAD * 2));
			col_text.setFillColor(sf::Color(colors[index]));

			// tri init
			sf::CircleShape* tri = tri_g.add(new sf::CircleShape()).set_ownership(true).get(index);
			tri->setRadius(COL_RAD);
			tri->setPointCount(3);
			tri->setOrigin(RAD_VECTOR);
			tri->move(col->getPosition() + sf::Vector2f(0.f, col->getHeight() + TRI_SPACING) + RAD_VECTOR);
			tri->setFillColor(col->getColor());

			// plot init
			plt_g.add((new Plotter({ PLT_LENGTH, PLT_HEIGHT }, BUFFER_SIZE))
				->setColor(col->getColor())
				.setBackgroundColor(sf::Color(PLT_BACKGROUND_COLOR))
			).set_ownership(true);
		}

		// post loop
		Plotter::format_window(plt_g.cast_inner<sf::Drawable>(), PLT_FORMAT_REF_POS, PLT_FORMAT_V_SPACING, PLT_FORMAT_H_SPACING, PLT_FORMAT_ROWS);
		plt_g.map_mut([](Plotter* plt) { plt->move(PLT_ADDITIONAL_SHIFT); });
		tri_g.map_mut([](sf::CircleShape* cs) { cs->move(TRI_ADDITIONAL_SHIFT); });
	}

	// Initialize the subcanvas
	static void init_canvas(SubCanvas*& canva, std::initializer_list<PointerVector<sf::Drawable>*> obj_list) {
		canva = new SubCanvas({ CANV_LENGTH, CANV_HEIGHT }, { 1280, 720 });
		for (PointerVector<sf::Drawable>* obj : obj_list) canva->link_group(obj);

		canva->setPosition(CANV_REF_POS - sf::Vector2f(canva->getDimensions().x / 2, canva->getDimensions().y / 2))
			.setBackgroundColor(sf::Color(CANV_BACKGROUND_COLOR))
			.set_outline_color(sf::Color(CANV_OUTLINE_COLOR))
			.set_outline_thickness(5.f)
			.set_shadow(true);
	}

	// should just update the heights based on the current state
	static void reset_columns(PointerVector<ColumnShape>& col_g) {
		const sf::Vector2f TRUE_REF_POS = COL_REF_POS - sf::Vector2f(COL_RAD * 2 * COL_SPACING_FACTOR * 2 - COL_RAD, COL_BASE_HEIGHT + COL_RAD * 3);
		for (uint8_t index = 0; index < 4; index++) {
			col_g.get(index)->setPosition(TRUE_REF_POS + sf::Vector2f(index * COL_RAD * 2 * COL_SPACING_FACTOR, 0.f))
				.setHeight(COL_BASE_HEIGHT);
		}
	}

	// initializes the equilibrium equation text
	static void init_equilibrium_equation_text(sf::Font*& eq_font, sf::Text*& eq_text, EQUILIBRIUM_STATES state) {
		eq_font = new sf::Font();
		if (!eq_font->loadFromFile(FONT_PATH_EQUILIBRIUM)) {
			std::printf("Can't load the path %s for some arbitrary reason..., exiting.\n", FONT_PATH_EQUILIBRIUM);
			exit(-1);
		}

		eq_text = new sf::Text();
		eq_text->setFont(*eq_font);
		eq_text->setPosition(TEXT_REF_POS);
		eq_text->setCharacterSize(36);
		eq_text->setString(EQUILIBRIUM_STATE_NAMES[state]);
		eq_text->setFillColor(sf::Color(0x000000FF));
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
		data_stream(BUFFER_SIZE), eq_state(state) {
		init_populations(this->pops);
		init_equilibrium_equation_text(eq_fnt, eq_txt, (EQUILIBRIUM_STATES) this->eq_state);
		init_drawables(this->col_group, this->tri_group, this->plot_group, this->pop_text_group, this->col_text_group, *this->eq_fnt);

		init_canvas(this->main_canvas, {
			&this->col_group.cast_inner<sf::Drawable>(),
			&this->tri_group.cast_inner<sf::Drawable>(),
			&this->plot_group.cast_inner<sf::Drawable>(),
			&this->pop_text_group.cast_inner<sf::Drawable>(),
			&this->col_text_group.cast_inner<sf::Drawable>()
		});

		this->pop_stock_maxes = { 
			BASE_COMMONER_POP * 10, 
			BASE_COMMONER_POP * 10, 
			BASE_NATURE_CARRY_CAPACITY, 
			500.f
		};
	}

public:
	// A singleton class, returns an instance of the HANDYManager.
	static HANDYManager& get_instance() { // singleton object -> only one instance of this class
		if (manager_instance == NULL) manager_instance = new HANDYManager(EQUILIBRIUM_STATES::EGALITARIAN_SOFT_LANDING);
		return *manager_instance;
	}

	// Destructor
	~HANDYManager() {
		for (int i = 0; i < 4; i++) delete this->pops[i];
		delete this->main_canvas;
		delete this->eq_txt;
		delete this->eq_fnt;
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

		this->pop_stock_maxes = lalg::make_vec4(BASE_COMMONER_POP * 10, BASE_COMMONER_POP * 10, BASE_NATURE_CARRY_CAPACITY, 500.f);
		this->data_stream.reset();
		this->eq_txt->setString(EQUILIBRIUM_STATE_NAMES[(EQUILIBRIUM_STATES)this->eq_state]);

		// clear ringbuffer
		reset_columns(col_group);

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
		this->pop_stock_maxes.r = /*std::fmaxf(this->pop_stock_maxes.g, (std::fmaxf(pop_vec.g, pop_vec.r)));*/ std::fmaxf(pop_vec.r, this->pop_stock_maxes.r);
		this->pop_stock_maxes.g = /*this->pop_stock_maxes.r;*/ std::fmaxf(pop_vec.g, this->pop_stock_maxes.g);
		this->pop_stock_maxes.a = std::fmaxf(pop_vec.a, this->pop_stock_maxes.a);
		return *this;
	}

	// use euler's method of integration, compute the integral of each population for the current state
	HANDYManager& compute_stocks() {
		lalg::vec4 pop_vec = lalg::zeroVec();
		for (size_t p = 0; p < 4; p++) lalg::set_index(pop_vec, (uint8_t) p, calcStock(*this->pops[p], 0, 0.5f, true).stock);

		//pop_vec.b = fmaxf(-1.f, pop_vec.b);
		//this->pops[3]->stock = pop_vec.b;

		this->data_stream.insert(pop_vec);
		return update_maxes(pop_vec);
	}

	static std::string float_cutoff(const std::string& in, int decimal_points) {
		int offset = 0;
		char cur_char;
		while (offset < in.size() && (cur_char = in[offset]) != '.') offset++;
		//in[std::min((int)in.size(), offset + decimal_points + 1)] = '\0';
		return in.substr(0, std::min((int)in.size(), offset + decimal_points + 1));
	}

	// Update all our drawable object groups to be ready for rendering
	HANDYManager& update_drawables() {
		const lalg::vec4 nouveau_stock_heights = lalg::diag(lalg::map(this->pop_stock_maxes, [](float f) {
			return 1.f / f;
		})) * this->data_stream.peek() * COL_MAX_HEIGHT;

		const lalg::vec4 diffs = nouveau_stock_heights - lalg::vec4({
			this->col_group.get(0)->getHeight(),
			this->col_group.get(1)->getHeight(),
			this->col_group.get(2)->getHeight(),
			this->col_group.get(3)->getHeight()
		});

		const float TOTAL_VERTICES = (float)((BUFFER_SIZE - 2) << 2);
		for (uint8_t index = 0; index < 4; index++) {
			// column
			ColumnShape& cur_col = *this->col_group.get(index);
			cur_col.setHeight(lalg::getValue(nouveau_stock_heights, index))
				.setPosition(cur_col.getPosition() - sf::Vector2f(0, lalg::getValue(diffs, index)));

			// pop text
			sf::Text& cur_pop_text = *this->pop_text_group.get(index);
			cur_pop_text.setPosition(cur_col.getPosition() + sf::Vector2f(0.f, std::fminf(cur_col.getHeight() + COL_RAD, cur_pop_text.getLocalBounds().getSize().x + COL_RAD)));

			// col text
			sf::Text& cur_col_text = *this->col_text_group.get(index);
			cur_col_text.setString(float_cutoff(std::to_string(lalg::getValue(this->data_stream.peek(), index)), index < 2 ? -1 : 2));
			cur_col_text.setPosition(cur_col.getPosition() - sf::Vector2f(cur_col_text.getLocalBounds().getSize().x / 2 - COL_RAD, COL_RAD * 2));

			// tri
			population& cur = *(pops[index]);
			this->tri_group.get(index)->setRotation((abs(cur.flow) == 0.f) * 90.f + (cur.flow < 0) * 180.f);

			// plotter
			const float POP_MAX_VAL = lalg::getValue(this->pop_stock_maxes, index);
			plot_group.get(index)->setVertices(this->data_stream, index, {
				TOTAL_VERTICES,
				TOTAL_VERTICES,
				POP_MAX_VAL,
				POP_MAX_VAL
			});
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

	// returns the column object group
	PointerVector<ColumnShape>& get_col_group() {
		return this->col_group;
	}

	// returns the canvas
	SubCanvas& get_canvas() {
		return *this->main_canvas;
	}

	sf::Text& get_eq_text() {
		return *this->eq_txt;
	}

	// handles da events
	HANDYManager& handle_events(sf::Event& event) {
		if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Left) {
				this->print_col_pos().print_flows().print_stocks().decrement_state();
			}

			if (event.key.code == sf::Keyboard::Right) {
				this->print_col_pos().print_flows().print_stocks().increment_state();
			}

			if (event.key.code == sf::Keyboard::Space) {
				ColumnShape::ColumnType col_type = this->col_group.get(0)->get_column_type();

				switch (col_type) {
					case ColumnShape::ColumnType::BAR:
						col_type = ColumnShape::ColumnType::PILLAR;
						break;
					case ColumnShape::ColumnType::PILLAR:
						col_type = ColumnShape::ColumnType::BAR;
						break;
				}

				this->col_group.map_capture([&col_type](ColumnShape* cs) { cs->set_column_type(col_type); });
			}
		}

		return *this;
	}

private:
	static HANDYManager* manager_instance; // for cool singleton stuff
};

HANDYManager* HANDYManager::manager_instance = nullptr;