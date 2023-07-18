#include "Domain.h"
#include <iostream>

// initializations (other)
const char* shader_path = "shaders/fragment/OpenDomain.hlsl";
sf::Shader* OpenDomain::odShader = NULL;
static long od_count = 0; // keep track of how many open domains currently exist

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
	od_count++;
}

OpenDomain::~OpenDomain() {
	od_count--;
	if (!od_count) delete odShader;
}

std::vector<sf::Vector2f> OpenDomain::getPointPairs() {
	std::vector<sf::Vector2f> points(2);
	int total_points = circle.getPointCount();
	int inverse_shift = total_points / 2;

	int cur = 0;
	/*points[0] = circle.getPoint(cur);
	points[1] = circle.getPoint(inverse_shift);*/

	/*cur = (int)(total_points * .125f);
	points[2] = circle.getPoint(cur);
	points[3] = circle.getPoint(cur + inverse_shift);*/

	float index_approx = total_points * .125f;
	/*std::cout << "total_points * .25: " << index_approx << std::endl;
	std::cout << "index: " << std::ceil(index_approx) << std::endl;
	std::printf("ceil (%f,%f)\n", circle.getPoint(std::ceil(index_approx)).x, circle.getPoint(std::ceil(index_approx)).y);*/
	cur = (int) (total_points * .125f);

	points[0] = circle.getPoint(cur);
	points[1] = circle.getPoint(cur + inverse_shift);

	/*std::printf("no ceil (%f,%f)\n", points[0].x, points[0].y);
	std::printf("no ceil inverse shift (%f,%f)\n", points[1].x, points[1].y);*/

	return points;
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