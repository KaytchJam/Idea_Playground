#include "Domain.h"
#include <iostream>

// initializations (other)
const char* shader_path = "shaders/fragment/OpenDomain.hlsl";
sf::Shader* OpenDomain::odShader = NULL;

OpenDomain::OpenDomain(float radius, sf::Color color, float refine_val, sf::Vector2f originCoords) : Domain(radius, color, refine_val, originCoords) {
	type = DomainType::OPEN_DOMAIN;
	circle.setPosition(originCoords);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setOutlineThickness(5.f);
	circle.setScale(sf::Vector2f(1, 1));
	base_origin_position = circle.getPosition();

	if (odShader == NULL) {
		odShader = new sf::Shader();
		odShader->loadFromFile(shader_path, sf::Shader::Fragment);
		std::cout << "Shader loaded." << std::endl;
	}
	else {
		std::cout << "Shader already loaded" << std::endl;
	}
}

void OpenDomain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::Vector2f center = getCenterCoords();
	odShader->setUniform("center", sf::Vector2f(0.f, target.getSize().y) - sf::Vector2f(-1 * center.x, center.y));
	odShader->setUniform("radius", getRadius());
	odShader->setUniform("thickness", getOutlineThickness());
	odShader->setUniform("line_color", sf::Glsl::Vec4(line_color));

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
	states.shader = odShader;
	target.draw(circle, states);
}