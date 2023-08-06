#include "Domain.h"

ClosedDomain::ClosedDomain(float radius, sf::Color color, float refine_val, sf::Vector2f originCoords) : Domain(radius, color, refine_val, originCoords) {
	type = DomainType::CLOSED_DOMAIN;
	circle.setPosition(originCoords);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setOutlineThickness(10.f);
	circle.setScale(sf::Vector2f(1, 1));
	base_origin_position = circle.getPosition();
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

void ClosedDomain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(circle, states);

	sf::VertexArray circLines(sf::Lines, 4);

	// VERTICAL LINE
	circLines[0].position = circle.getPosition() + circle.getPoint(0);
	circLines[0].color = line_color;
	circLines[1].position = circle.getPosition() + circle.getPoint(0) + sf::Vector2f(0, circle.getRadius());
	circLines[1].color = line_color;

	// HORIZONTAL LINE
	circLines[2].position = circle.getPosition() + sf::Vector2f(0, circle.getRadius());
	circLines[2].color = line_color;
	circLines[3].position = circle.getPosition() + circle.getPoint(0) + sf::Vector2f(0, circle.getRadius());
	circLines[3].color = line_color;

	target.draw(circLines, states);
}