#pragma once
#include "../Populations.h"
#include "lalg4.hpp"
#include "RingBuffer.h"
#include "PointerVector.h"
#include "../CustomShapes/ColumnShape.h"
#include "../CustomShapes/Plotter.h"
#include "../CustomShapes/SubCanvas.h"

void calcAllFlows(human_pop& elite, human_pop& commoner, nature_stock& nat, wealth_stock& wealth) {
	calcFlow(elite);
	calcFlow(commoner);
	calcFlow(nat, commoner);
	calcFlow(wealth, nat, elite, commoner);
}

lalg::vec4 calcAllStocks(human_pop& elite, human_pop& commoner, nature_stock& nat, wealth_stock& wealth) {
	return {
			calcStock(elite, 0, 1, true).stock,
			calcStock(commoner, 0, 1, true).stock,
			calcStock(nat, 0, 1, true).stock,
			calcStock(wealth, 0, 1, true).stock
	};
}

void update_maxes(lalg::vec4& max_vector, lalg::vec4& cur_stocks) {
	const float hpop_max = std::max(cur_stocks.r, cur_stocks.g);
	max_vector.r = std::max(max_vector.r, cur_stocks.r);
	max_vector.g = std::max(max_vector.g, cur_stocks.g);
	max_vector.a = std::max(max_vector.a, cur_stocks.a);
}

static sf::Vector2f lerp(const sf::Vector2f start, const sf::Vector2f end, const int numerator, const int denominator) {
	float frac = (float)numerator / denominator;
	return start * (1 - frac) + end * (frac);
}

static lalg::vec4 lerp(const lalg::vec4 start, const lalg::vec4 end, const int numerator, const int denominator) {
	float frac = (float)numerator / denominator;
	return start * (1 - frac) + end * (frac);
}

// normalizes value in a range, assume RANGE.x is the lower bound and RANGE.y is the upper bound
static float normalize(const float val, const sf::Vector2f RANGE) {
	return (val - RANGE.x) / (RANGE.y - RANGE.x);
}

static lalg::vec4 normalize(const lalg::vec4& vals, const sf::Vector2f RANGE) {
	return (vals - RANGE.x * lalg::ones_vec()) / (RANGE.y - RANGE.x);
}

//static lalg::vec4 normalizeL(const lalg::vec4& vals, const lalg::vec4& maxes) {
//	return lalg::map(lalg::diag(maxes), [](float f) { return 1 / (f + (f == 0.0f)) - (f == 0.0f); }) * vals;
//}

static lalg::vec4 normalizeL(const lalg::vec4& vals, const lalg::vec4& maxes) {
	return lalg::diag(lalg::map(maxes, [](float f) { return 1 / (f + 1 * (f == 0.f)); })) * vals;
}

static float normToRange(const float val, const sf::Vector2f OLD_RANGE, const sf::Vector2f NEW_RANGE) {
	return normalize(val, OLD_RANGE) * (NEW_RANGE.y - NEW_RANGE.x) + NEW_RANGE.x;
}

static lalg::vec4 sfToLalg(const sf::Vector2f& sfVec) {
	return { sfVec.x, sfVec.y, 1, 1 };
}

static sf::Vector2f lalgToSf(const lalg::vec4& v) {
	return { v.r, v.g };
}

void init_columns(PointerVector<sf::Drawable>& cg, float c_rad, float c_height, float c_spacing, uint32_t CCs[4], sf::Vector2f ref_pos) {
	for (int i = 0; i < cg.size(); i++) {
		ColumnShape* col_ptr = (ColumnShape*) cg.get(i);

		col_ptr->setRadius(c_rad)
			.setHeight(c_height)
			.setPosition(ref_pos + sf::Vector2f((-2 + i) * c_spacing, 0))
			.setColor(CCs[i])
			.set_column_type(ColumnShape::ColumnType::BAR);
	}
}

void init_tris(PointerVector<sf::Drawable>& tris, PointerVector<sf::Drawable> cols, float rad, float spacing) {
	for (int i = 0; i < tris.size(); i++) {
		sf::CircleShape* tri = (sf::CircleShape*) tris.get(i);
		ColumnShape* curCol = (ColumnShape*) cols.get(i);

		tri->setRadius(rad);
		tri->setPointCount(3);
		tri->setOrigin({ rad,rad });
		tri->move(curCol->getPosition() + sf::Vector2f(0.f, curCol->getHeight() + spacing) + sf::Vector2f(rad, rad));
		tri->setFillColor(curCol->getColor());
	}
}

float sf_magnitude(const sf::Vector2f& v) {
	return std::sqrtf(std::powf(v.x, 2) + std::powf(v.y, 2));
}

sf::Vector2f bind_mouse_position(sf::Vector2i local_pos, int WIN_LENGTH, int WIN_HEIGHT) {
	int call = 0;
	return lalgToSf(lalg::map_capture(sfToLalg({ (float)local_pos.x, (float)local_pos.y }), [&WIN_LENGTH, &WIN_HEIGHT, &call](float f) {
		return std::fmaxf(0.f, std::fminf(f, ((float)WIN_LENGTH * !(call == 1)) + ((float)WIN_HEIGHT * (call++ == 1)))); }));
}

sf::Vector2f get_global_force(const sf::Vector2f& local_pos, int WIN_LENGTH, int WIN_HEIGHT, float FORCE_MULTIPLIER) {
	const lalg::mat4 bound_mouse_coords = 
	   {{ 2.f / WIN_LENGTH, 0.f, -1.f, 0.f},
		{ 0.f, 2.f / WIN_HEIGHT,  -1.f,  0.f },
		{ 0.f, 0.f,                1.f,  0.f },
		{ 0.f, 0.f,                0.f,  1.f } };
	float call = 0;
	return lalgToSf(bound_mouse_coords * lalg::map_capture(sfToLalg({ (float) local_pos.x, (float) local_pos.y }), [&WIN_LENGTH, &WIN_HEIGHT, &call](float f) {
				return std::fmaxf(0, std::fminf(f, ((float)WIN_LENGTH * !(call == 1)) + (WIN_HEIGHT * (call++ == 1)))); }) * FORCE_MULTIPLIER);
}

sf::Vector2f get_global_force(const sf::Vector2i& mouse_pos, int WIN_LENGTH, int WIN_HEIGHT, float FORCE_MULTIPLIER) {
	sf::Vector2f local_pos = sf::Vector2f((float) mouse_pos.x, (float) mouse_pos.y);
	return get_global_force(local_pos, WIN_LENGTH, WIN_HEIGHT, FORCE_MULTIPLIER);
}

sf::Vector2f print_return(const sf::Vector2f v2) {
	std::printf("{%f,%f}\n", v2.x, v2.y);
	return v2;
}

sf::Vector2f get_relative_force(const sf::Vector2f& bound_pos, const SubCanvas& canva, float FORCE_MULTIPLIER) {
	sf::Vector2f diff = bound_pos - canva.getCenter();
	float mag = sf_magnitude(diff);
	return mag == 0.f ? sf::Vector2f(0.f, 0.f) : sf::Vector2f(diff.x / mag * FORCE_MULTIPLIER, diff.y / mag * FORCE_MULTIPLIER );
}
