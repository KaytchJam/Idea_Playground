#pragma once


struct population {
	float stock;	// x
	float flow;	// dx/dt
};

struct human_pop : population {
	//float stock							// X - current population
	//float  D_stock						// X - change in population dx/dt

	float birth_rate;						// B - birth rate
	float death_rate;						// a - death rate

	float output_portion_K;					// K - division of output given to this population
	float mcpc_p;							// p - minimum consumption per capita

	float consumption;						// C - consumption of population
	float sspc_s;							// s - subsistence salary per capita
};

struct nature_stock : population {
	//float stock							// y  - current nature stock
	//float D_stock;						// y' - change in nature stock dy/dt
	float regeneration_factor;				// r - rate at which nature stock regrows
	float depletion_per_worker;				// d - rate of depletion of nature stock per worker
	float capacity;							// v - stock capacity factor
};

struct wealth_stock : population {
	//float stock;							// w - current wealth stock
	//float D_wealth;						// w' - change in wealth stock dw/dt
	float wealth_threshold;					// w_thresh - threshold wealth needed to pay for consumption
};

// Initializers
void init_human_pop(human_pop& hp, float initial_pop, float br, float k, float P, float s);
void init_nature(nature_stock& nst, float initial_stock, float regen, float depletion, float capacity);
void init_wealth(wealth_stock& wst, float initial_stock, float threshold);

/**
Differential Equations :
.
-> X.c' = B.c * X.c - a.c * x.c
-> X.e' = B.e * X.e - a.e * x.e
-> y'   = r * y * (v - y) - v * x.c * y
-> w'   = d * x.c * y - C.c - C.e
.
**/

// CALCULATE DERIVATIVES
human_pop& calcFlow(human_pop& hp);
nature_stock& calcFlow(nature_stock& nst, human_pop& commoner_hp);
wealth_stock& calcFlow(wealth_stock& wst, nature_stock& nst, human_pop& elite_hp, human_pop& commoner_hp);

// CALCULATE SOLUTIONS (STOCKS)
population& calcStock(population& pop, const float begin, const float end, const bool use_prev);

// SIMULATION
//int runSimToFile();

// BASE INITIAL VALUES:
const float BASE_MIN_DEATH_RATE = (float)1.e-2;
const float BASE_MAX_DEATH_RATE = (float)7e-2;
const float BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
const float BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
const float BASE_SSPC = (float)5.0e-4;
const float BASE_TWPC = (float)5.0e-3;
const float BASE_REGENERATION_RATE = (float)1e-2;
const float BASE_NATURE_CARRY_CAPACITY = (float)1.0e2;
const float BASE_INEQUALITY_FACTOR = 10.f;
const float BASE_DEPLETION_FACTOR = (float)1.67e-5; //6.67e-6;

const float BASE_COMMONER_POP = (float)1e2;
const float BASE_ELITE_POP = 0.f;
const float BASE_NATURE_STOCK = BASE_NATURE_CARRY_CAPACITY;
const float BASE_WEALTH_STOCK = 0.f;

static float HP_HEALTHY_VAL = BASE_MIN_DEATH_RATE;
static float HP_FAMINE_VAL = BASE_MAX_DEATH_RATE;

// EQUILIBRIUM VALUES: