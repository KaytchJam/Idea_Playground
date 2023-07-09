#include "Domain.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

static float deg2rad(float val) {
	return val / 360.f * 2 * M_PI;
}

ClosedDomain::ClosedDomain(float radius, sf::Color color, float refine_val, sf::Vector2f centerCoords) : refinement(refine_val), base_radius(radius) {
	circle = sf::CircleShape(radius);
	circle.setPosition(centerCoords);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setScale(sf::Vector2f(1, 1));
}

void ClosedDomain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(circle, states);
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

bool ClosedDomain::inRange(ClosedDomain& other) {
	return (distance(other) - other.getTrueRadius()) <= this->getTrueRadius();
}

void ClosedDomain::consume(ClosedDomain& other) {

}

void ClosedDomain::onUpdate() {

	if (IDLE) {
		float radians = deg2rad(degree);

		//std::cout << "DEGREE: " << degree << std::endl;
		////std::cout << "SCALE VECTOR: " << circle.getScale().x << ", " << circle.getScale().y << std::endl;
		//std::cout << "RADIANS: " << radians << std::endl;
		//std::cout << std::endl;

		float offset = std::sin(radians);

		circle.setRadius(base_radius + offset);
		degree = degree < 360 ? degree + DEGREES_PER_FRAME : 0;
	}
}

std::ostream& operator<<(std::ostream& stream, const ClosedDomain& d) {
	sf::Vector2f center = d.getCenterCoords();
	return stream << "(x: " << center.x << ", y: " << center.y << ", r: " << d.getRadius() << ")";
}
