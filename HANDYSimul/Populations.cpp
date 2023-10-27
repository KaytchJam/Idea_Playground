#include "Populations.h"
#include <cmath>

// initializes our human population stocks...
void init_human_pop(human_pop& p, float initial_pop, float br, float k, float P, float s) {
	p.stock = initial_pop;
	p.D_pop = 0.f;
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
	nst.D_stock = 0.f;
	nst.regeneration_factor = regen;
	nst.depletion_per_worker = depletion;
	nst.capacity = capacity;
}

// initializes our wealth stock ...
void init_wealth(wealth_stock& wst, float initial_stock, float thresh) {
	wst.stock = initial_stock;
	wst.D_wealth = 0.f;
	wst.wealth_threshold = thresh;
}

human_pop& hp_consumption_rate(human_pop& hp, const wealth_stock& wst) {
	hp.consumption = fmin(1, wst.stock / wst.wealth_threshold) * hp.output_portion_K * hp.sspc_s * hp.stock;
	return hp;
}

human_pop& hp_death_rate(human_pop& hp) {
	hp.death_rate = HP_HEALTHY_VAL + fmax(0, 1 - hp.consumption / (hp.sspc_s * hp.stock)) * (HP_FAMINE_VAL - HP_HEALTHY_VAL);
	return hp;
}

wealth_stock& wst_wealth_threshold(wealth_stock& wst, const human_pop& elite, const human_pop& commoner) {
	wst.wealth_threshold = elite.mcpc_p * elite.stock * commoner.output_portion_K + commoner.mcpc_p * commoner.stock * commoner.output_portion_K;
	return wst;
}
