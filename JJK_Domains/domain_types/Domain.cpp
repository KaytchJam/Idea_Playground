#include "Domain.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

static float deg2rad(float val) {
	return (float) (val / 360.f * 2 * M_PI);
}

ClosedDomain::ClosedDomain(float radius, sf::Color color, float refine_val, sf::Vector2f originCoords) : refinement(refine_val), base_radius(radius), line_color(color) {
	circle = sf::CircleShape(radius);
	circle.setPosition(originCoords);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setScale(sf::Vector2f(1, 1));
	base_origin_position = circle.getPosition();
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

	target.draw(circLines);
}

sf::Vector2f ClosedDomain::getCenterCoords() const {
	sf::Vector2f pos = circle.getPosition();
	return pos + sf::Vector2f(circle.getRadius(), circle.getRadius());
}

float ClosedDomain::distance(ClosedDomain& other) {
	sf::Vector2f oCenter = other.getCenterCoords();
	sf::Vector2f thisCenter = this->getCenterCoords();
	return std::sqrtf(std::powf(oCenter.x - thisCenter.x, 2) + std::powf(oCenter.y - thisCenter.y, 2));
}

void ClosedDomain::setCenterPosition(sf::Vector2f pos) {
	circle.setPosition(pos - sf::Vector2f(base_radius, base_radius));
	base_origin_position = circle.getPosition();
}

bool ClosedDomain::inRange(ClosedDomain& other) {
	return (distance(other) - other.getTrueRadius()) <= this->getTrueRadius();
}

void ClosedDomain::consume(ClosedDomain& other) {

}

void ClosedDomain::onUpdate(float deltaTime) {
	//std::cout << "degree: " << degree << std::endl;
	if (IDLE) {
		float offset = std::sin(deg2rad(degree));
		circle.setRadius(base_radius + offset);
		circle.setPosition(base_origin_position + sf::Vector2f(-1 * offset, -1 * offset));
		//std::cout << "OFFSET: " << offset << std::endl;
		degree = degree < 360 ? degree + (DEGREES_PER_FRAME) * deltaTime : 0;
	}
}

std::ostream& operator<<(std::ostream& stream, const ClosedDomain& d) {
	sf::Vector2f center = d.getCenterCoords();
	return stream << "(x: " << center.x << ", y: " << center.y << ", r: " << d.getRadius() << ")";
}
