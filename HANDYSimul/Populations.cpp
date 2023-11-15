#include "Populations.h"
#include <cmath>

// initializes our human population stocks...
void init_human_pop(human_pop& p, float initial_pop, float br, float k, float P, float s) {
	p.stock = initial_pop;
	p.flow = 0.f;
	p.birth_rate = br;
	p.death_rate = 0.f;
	p.output_portion_K = k;
	p.consumption = 0.f;
	p.mcpc_p = P;
	p.sspc_s = s;
}

// initializes our nature stocks...
void init_nature(nature_stock& nst, float initial_stock, float regen, float depletion, float capacity) {
	nst.stock = initial_stock;
	nst.flow = 0.f;
	nst.regeneration_factor = regen;
	nst.depletion_per_worker = depletion;
	nst.capacity = capacity;
}

// initializes our wealth stock ...
void init_wealth(wealth_stock& wst, float initial_stock, float thresh) {
	wst.stock = initial_stock;
	wst.flow = 0.f;
	wst.wealth_threshold = thresh;
}

// consumption rate calculation
human_pop& hp_consumption_rate(human_pop& hp, const wealth_stock& wst) {
	hp.consumption = (float) fmin(1, wst.stock / wst.wealth_threshold) * hp.output_portion_K * hp.sspc_s * hp.stock;
	return hp;
}

// death rate calculation
human_pop& hp_death_rate(human_pop& hp) {
	hp.death_rate = HP_HEALTHY_VAL + (float) fmax(0, 1 - hp.consumption / (hp.sspc_s * hp.stock)) * (HP_FAMINE_VAL - HP_HEALTHY_VAL);
	return hp;
}

// wealth threshold calculation
wealth_stock& wst_wealth_threshold(wealth_stock& wst, const human_pop& elite, const human_pop& commoner) {
	wst.wealth_threshold = elite.mcpc_p * elite.stock * elite.output_portion_K + commoner.mcpc_p * commoner.stock * commoner.output_portion_K;
	return wst;
}

/** DIFFERENTIAL EQUATIONS 
	We'll be taking the "integral" of these to compute our stocks at each time interval.
	Luckily for us these are autonomous differential equations, our time interval
	shouldn't have too much bearing on our result.
**/

// HUMAN POPULATIONS (COMMONER / ELITE ) : x' = beta * x - alpha * x
human_pop& calcFlow(human_pop& hp) {
	hp.flow = hp.birth_rate * hp.stock - hp_death_rate(hp).death_rate * hp.stock;
	return hp;
}

// NATURE STOCK : y' = gamma * y * (lambda - y) - delta * x_c * y
nature_stock& calcFlow(nature_stock& nst, human_pop& commoner_hp) {
	nst.flow = nst.regeneration_factor * nst.stock * (nst.capacity - nst.stock) 
		- nst.depletion_per_worker * commoner_hp.stock * nst.stock;
	return nst;
}

// WEALTH STOCK : w' = delta * x_c * y - C_c - C_e
wealth_stock& calcFlow(wealth_stock& wst, nature_stock& nst, human_pop& elite_hp, human_pop& commoner_hp) {
	wst_wealth_threshold(wst, elite_hp, commoner_hp);
	wst.flow = nst.depletion_per_worker * commoner_hp.stock * nst.stock 
		- hp_consumption_rate(commoner_hp, wst).consumption - hp_consumption_rate(elite_hp, wst).consumption;
	return wst;
}

// compute differential equation
// "take the reimann of it"

// run this after calculating our flow (dx/dt)
population& calcStock(population& pop, const float begin, const float end, const bool use_prev) {
	const float dt = 0.001f;
	float sum = 0.0f + pop.stock * use_prev; // for initial condition
	float t = begin;

	// """Integral"""
	while (t < end) {
		sum += pop.flow * dt; // dx/dt * dt
		t += dt;
	}

	bool eq_bounds = begin == end;
	pop.stock = sum * !eq_bounds + pop.stock * eq_bounds; // check in case our bounds are equal
	return pop;
}