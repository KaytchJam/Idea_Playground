#include "DomainManager.h"

ClosedDomain::ClosedDomain(float radius, sf::Color color, float refine_val, sf::Vector2f centerCoords) : Domain(radius, color, refine_val, centerCoords) {
	type = DomainType::CLOSED_DOMAIN;
}

ClosedDomain::~ClosedDomain() {

}

const int REDUCTION_FACTOR = 5;
void ClosedDomain::consume(Domain& other) {
	if (!CONSUMED) {
		float refine_diff = this->refinement - other.getRefinement();
		//std::cout << refine_diff << std::endl;
		float factor = refine_diff * 10;
		base_radius += factor;
		circle.setRadius(base_radius);
		circle.setPosition(base_origin_position - sf::Vector2f(factor, factor));
	}

	CONSUMED = base_radius <= 0 ? true : false;
}