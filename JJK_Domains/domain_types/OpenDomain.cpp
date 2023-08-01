#include "Domain.h"
#include <iostream>

// initializations (other)
const char* shader_path = "shaders/fragment/OpenDomain.frag";
sf::Shader* OpenDomain::odShader = NULL;
static long od_count = 0; // keep track of how many open domains currently exist

OpenDomain::OpenDomain(float radius, sf::Color color, float refine_val, sf::Vector2f originCoords) : Domain(radius, color, refine_val, originCoords) {
	type = DomainType::OPEN_DOMAIN;
	circle.setPosition(originCoords);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineColor(color);
	circle.setOutlineThickness(10.f);
	circle.setScale(sf::Vector2f(1, 1));
	base_origin_position = circle.getPosition();

	float total_points = (float) circle.getPointCount();
	point_indices[0] = 0;
	point_indices[1] = total_points * .125f;
	point_indices[2] = total_points * .25f;
	point_indices[3] = total_points * .375f;

	if (odShader == NULL) {
		odShader = new sf::Shader();
		odShader->loadFromFile(shader_path, sf::Shader::Fragment);
		std::cout << "Shader loaded." << std::endl;
	} else {
		std::cout << "Shader already loaded" << std::endl;
	}
	od_count++;
}

OpenDomain::~OpenDomain() {
	od_count--;
	if (!od_count) delete odShader;
}

void OpenDomain::idleStates(float deltaTime) {
	for (int i = 0; i < 4; i++) {
		point_indices[i] = point_indices[i] >= circle.getPointCount() - 1 ? 0 : point_indices[i] + 10.f * deltaTime;
	}
}

static sf::Vector3f getStandardCoefs(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f offset) {
	float slope = (p2.y - p1.y) / (p2.x - p1.x);
	float intercept = (p2.y + offset.y) - (slope * (p2.x + offset.x));
	return sf::Vector3f(-1 * slope, 1, -1 * intercept);

}

void OpenDomain::consume(Domain& other) {
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

void OpenDomain::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::Vector2f center = getCenterCoords();
	float radius = getRadius();
	sf::Vector2f local_center = sf::Vector2f(radius, radius);
	sf::Vector3f standardList[4];

	sf::CircleShape totem(radius / 10);
	totem.setFillColor(line_color);
	totem.setPosition(centerToOriginCoords(center, totem.getRadius()));
	target.draw(totem, states);

	for (int i = 0; i < 4; i++) {
		standardList[i] = getStandardCoefs(local_center, circle.getPoint((int)point_indices[i]), getOriginCoords());
	}

	odShader->setUniform("center", sf::Vector2f(0.f, (float) target.getSize().y) - sf::Vector2f(-1 * center.x, center.y));
	odShader->setUniform("radius", getRadius());
	odShader->setUniform("line_color", sf::Glsl::Vec4(line_color));
	odShader->setUniform("resolution", sf::Vector2f( (float) target.getSize().x, (float) target.getSize().y));
	odShader->setUniform("TUG_OF_WAR", TUG_OF_WAR);
	odShader->setUniformArray("standard", standardList, 4);


	//sf::VertexArray circLines(sf::Lines, 4);

	// VERTICAL LINE
	//circLines[0].position = circle.getPosition() + circle.getPoint(0);
	//circLines[0].color = line_color;
	//circLines[1].position = circle.getPosition() + circle.getPoint(0) + sf::Vector2f(0, circle.getRadius());
	//circLines[1].color = line_color;

	//// HORIZONTAL LINE
	//circLines[2].position = circle.getPosition() + sf::Vector2f(0, circle.getRadius());
	//circLines[2].color = line_color;
	//circLines[3].position = circle.getPosition() + circle.getPoint(0) + sf::Vector2f(0, circle.getRadius());
	//circLines[3].color = line_color;

	//target.draw(circLines, states);
	states.shader = odShader;
	target.draw(circle, states);
}