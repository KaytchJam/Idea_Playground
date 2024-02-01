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
static float BASE_MIN_DEATH_RATE = (float)1.e-2;
static float BASE_MAX_DEATH_RATE = (float)7e-2;
static float BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
static float BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
static float BASE_SSPC = (float)5.0e-4;
static float BASE_TWPC = (float)5.0e-3;
static float BASE_REGENERATION_RATE = (float)1e-2;
static float BASE_NATURE_CARRY_CAPACITY = (float)1.0e2;
static float BASE_INEQUALITY_FACTOR = 10.f;
static float BASE_DEPLETION_FACTOR = (float)1.67e-5; //6.67e-6;

static float BASE_COMMONER_POP = (float)1e2;
static float BASE_ELITE_POP = 0.f;
static float BASE_NATURE_STOCK = BASE_NATURE_CARRY_CAPACITY;
static float BASE_WEALTH_STOCK = 0.f;

static float HP_HEALTHY_VAL = BASE_MIN_DEATH_RATE;
static float HP_FAMINE_VAL = BASE_MAX_DEATH_RATE;

enum EQUILIBRIUM_STATES {
	EGALITARIAN_SOFT_LANDING,
	EGALITARIAN_OSCILLATE_TOWARDS_EQUILIBRIUM,
	EGALITARIAN_CYCLES_AND_REVIVAL,
	EGALITARIAN_FULL_COLLAPSE,
	EQUITABLE_SOFT_LANDING,
	EQUITABLE_OSCILLATE_TOWARDS_EQUILIBRIUM,
	EQUITABLE_CYCLES_AND_REVIVAL,
	EQUITABLE_FULL_COLLAPSE,
	EQUITABLE_FULL_COLLAPSE_PREVENTION,
	UNEQUAL_TYPE_L_COLLAPSE,
	UNEQUAL_TYPE_N_COLLAPSE,
	UNEQUAL_SOFT_LANDING,
	UNEQUAL_OSCILLATE_TOWARDS_EQUILIBRIUM,
	TOTAL_STATES
};

const char* const EQUILIBRIUM_STATE_NAMES[13] = {
	"Egalitarian Society: Soft Landing to Equilibrium",
	"Egalitarian Society: Oscillatory Approach to Equilibrium",
	"Egalitarian Society: Cycles of Prosperity, Overshoot, Collapse, and Revival",
	"Egalitarian Society: Irreversible Type-N Collapse (Full Collapse)",
	"Equitable Society: Soft Landing to Optimal Equilibrium",
	"Equitable Society: Oscillatory Approach to Equilibrium",
	"Equitable Society: Cycles of Prosperity, Overshoot, Collapse, and Revival",
	"Equitable Society: Full Collapse",
	"Equitable Society: Preventing a Full Collapse by Decreasing Average Depletion per Capita",
	"Unequal Society: Type-L Collapse (Labor Disappears, Nature Recovers)",
	"Unequal Society: Irreversible Type-N Collapse (Full Collapse)",
	"Unequal Society: Soft Landing to Optimal Equilibrium",
	"Unequal Society: Oscillatory Approach to Equilibrium"
};

// EQUILIBRIUM VALUES:
static void setEquilibriumValues(const unsigned int index) {
	// EGALITARIAN SOCIETY: 0 - 3
	// EQUITABLE SOCIETY: 4 - 8
	// UNEQUAL SOCIETY: 9 - 12
	switch (index) {
		case EGALITARIAN_SOFT_LANDING:
			BASE_ELITE_POP = 0;
			BASE_DEPLETION_FACTOR = 6.67e-6f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			break;
		case EGALITARIAN_OSCILLATE_TOWARDS_EQUILIBRIUM: // 0 ELITES -> Oscillate towards equilibrium
			BASE_ELITE_POP = 0;
			BASE_DEPLETION_FACTOR = 1.67e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			break;
		case EGALITARIAN_CYCLES_AND_REVIVAL:
			BASE_ELITE_POP = 0;
			BASE_DEPLETION_FACTOR = 2.67e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			break;
		case EGALITARIAN_FULL_COLLAPSE:
			BASE_ELITE_POP = 0;
			BASE_DEPLETION_FACTOR = 3.67e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			break;
		case EQUITABLE_SOFT_LANDING:
			BASE_ELITE_POP = 25;
			BASE_INEQUALITY_FACTOR = 1.f;
			BASE_DEPLETION_FACTOR = 8.33e-6f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case EQUITABLE_OSCILLATE_TOWARDS_EQUILIBRIUM:
			BASE_ELITE_POP = 25;
			BASE_INEQUALITY_FACTOR = 1.f;
			BASE_DEPLETION_FACTOR = 2.20e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case EQUITABLE_CYCLES_AND_REVIVAL:
			BASE_ELITE_POP = 25;
			BASE_INEQUALITY_FACTOR = 1.f;
			BASE_DEPLETION_FACTOR = 3.e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case EQUITABLE_FULL_COLLAPSE: 
			BASE_ELITE_POP = 25;
			BASE_INEQUALITY_FACTOR = 1.f;
			BASE_DEPLETION_FACTOR = 4.33e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case EQUITABLE_FULL_COLLAPSE_PREVENTION:
			BASE_ELITE_POP = 600;
			BASE_INEQUALITY_FACTOR = 1.f;
			BASE_DEPLETION_FACTOR = 4.33e-5f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case UNEQUAL_TYPE_L_COLLAPSE:
			BASE_INEQUALITY_FACTOR = 100.f;
			BASE_DEPLETION_FACTOR = 6.67e-6f;
			BASE_ELITE_POP = 1e-3f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case UNEQUAL_TYPE_N_COLLAPSE:
			BASE_INEQUALITY_FACTOR = 100.f;
			BASE_DEPLETION_FACTOR = 1.e-4f;
			BASE_ELITE_POP = 0.2f;
			BASE_COMMONER_BIRTH_RATE = (float)3.0e-2;
			BASE_COMMONER_POP = (float)1e2;
			BASE_ELITE_BIRTH_RATE = BASE_COMMONER_BIRTH_RATE;
			break;
		case UNEQUAL_SOFT_LANDING:
			BASE_INEQUALITY_FACTOR = 10.f;
			BASE_COMMONER_BIRTH_RATE = 6.5e-2f;
			BASE_ELITE_BIRTH_RATE = 2e-2f;
			BASE_COMMONER_POP = 1e4f;
			BASE_ELITE_POP = 3e3f;
			BASE_DEPLETION_FACTOR = 6.35e-6f;
			break;
		case UNEQUAL_OSCILLATE_TOWARDS_EQUILIBRIUM:
			BASE_INEQUALITY_FACTOR = 10.f;
			BASE_COMMONER_BIRTH_RATE = 6.5e-2f;
			BASE_ELITE_BIRTH_RATE = 2e-2f;
			BASE_COMMONER_POP = 1e4f;
			BASE_ELITE_POP = 3e3f;
			BASE_DEPLETION_FACTOR = 1.35e-5f;
			break;
	}
}

