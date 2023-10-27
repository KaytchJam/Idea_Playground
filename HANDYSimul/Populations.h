#pragma once

struct human_pop {
	float stock;							// X - current population
	float D_pop;							// X - change in population dx/dt

	float birth_rate;						// B - birth rate
	float death_rate;						// a - death rate

	float output_portion_K;					// K - division of output given to this population
	float mcpc_p;							// p - minimum consumption per capita

	float consumption;						// C - consumption of population
	float sspc_s;							// s - subsistence salary per capita
};

struct nature_stock {
	float stock;							// y  - current nature stock
	float D_stock;							// y' - change in nature stock dy/dt
	float regeneration_factor;				// r - rate at which nature stock regrows
	float depletion_per_worker;				// d - rate of depletion of nature stock per worker
	float capacity;							// v - stock capacity factor
};

struct wealth_stock {
	float stock;							// w - current wealth stock
	float D_wealth;							// w' - change in wealth stock dw/dt
	float wealth_threshold;					// w_thresh - threshold wealth needed to pay for consumption
};

static float HP_HEALTHY_VAL = 15.f;
static float HP_FAMINE_VAL = 25.f;

// Initializers
void init_human_pop(human_pop& hp, float initial_pop, float br, float k, float P, float s);
void init_nature(nature_stock& nst, float initial_stock, float regen, float depletion, float capacity);
void init_wealth(wealth_stock& wst, float initial_stock, float threshold);
void calc_consumption_rate(human_pop& hp, const wealth_stock& wst);

/** 
Differential Equations :
.
-> X.c' = B.c * X.c - a.c * x.c
-> X.e' = B.e * X.e - a.e * x.e
-> y'   = r * y * (v - y) - v * x.c * y
-> w'   = d * x.c * y - C.c - C.e
.
**/


