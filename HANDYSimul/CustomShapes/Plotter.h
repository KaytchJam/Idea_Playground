#pragma once
#include "SFML/Graphics.hpp"
#include "../MyTools/RingBuffer.h"
#include "../MyTools/MyV.h"

class Plotter : public sf::Drawable {
private:
	unsigned int m_LENGTH;
	unsigned int m_HEIGHT;
	unsigned int m_POINTS_TO_RENDER;

	sf::Vector2f m_origin;
	sf::Color m_PLOT_COLOR;

	sf::RectangleShape bg;
	std::vector<sf::Vertex> m_vertices;
	
public:
	Plotter(sf::Vector2f size, unsigned int NUMBER_OF_POINTS, sf::Color line_color = sf::Color::Black, sf::Color bg_color = sf::Color::White) 
		: m_LENGTH((int) size.x), m_HEIGHT((int) size.y), m_POINTS_TO_RENDER(0) {
		m_vertices = std::vector<sf::Vertex>(NUMBER_OF_POINTS * 2);
		m_PLOT_COLOR = line_color;
		bg = sf::RectangleShape(size);
		bg.setFillColor(bg_color);
	}

	~Plotter() {

	}

	Plotter& setPosition(const sf::Vector2f& pos) {
		m_origin = pos;
		bg.setPosition(pos);
		return *this;
	}

	Plotter& setColor(const sf::Color& kalah) {
		m_PLOT_COLOR = kalah;
		return *this;
	}

	Plotter& setBackgroundColor(const sf::Color& kalah) {
		bg.setFillColor(kalah);
		return *this;
	}

	sf::Vector2f getSize() const {
		return { (float) m_LENGTH, (float) m_HEIGHT };
	}

	sf::Vector2f getPosition() const {
		return m_origin;
	}

	// min_vec = zeroVec(); // { 0, 0, 0, 0}
	// max_vec = { rb.size(), rb.size(), value_max, value_max };

	void setVertices(RingBuffer<lalg::vec4>& rb, unsigned int offset, const lalg::vec4& MAX_VEC, const lalg::vec4& MIN_VEC = lalg::zeroVec()) {
		using namespace lalg; // less typing 

		mat4 coord_transform;
		{
			vec4 diff_vec = MAX_VEC - MIN_VEC;
			const vec4 diagonal_inverse_vec = { diff_vec.r / m_LENGTH, diff_vec.g / m_LENGTH, diff_vec.b / m_HEIGHT, diff_vec.a / m_HEIGHT };
			coord_transform = diag(map(diagonal_inverse_vec, [](float f) {return 1.f / (f + 0.001f * (f == 0.0f));  }));
		}
		//vec4 target_maxes = { m_LENGTH, m_LENGTH, m_HEIGHT, m_HEIGHT };
		//const mat4 norm_each_elem_transform = diag(map(MAX_VEC - MIN_VEC, [](float f) {return 1.f / (f + 0.001f * (f == 0.0f));  }));
		//const mat4 to_graph_coords_transform = diag({ (float) m_LENGTH, (float) m_LENGTH, (float) m_HEIGHT, (float) m_HEIGHT });
		vec4 current_vec = { 0 /*prev index*/, 1 /*index + 1*/, getValue(rb.front(), offset) /*prev*/, 0 /*current*/ };

		RingBuffer<vec4>::iterator it = rb.begin();
		while (it != rb.end() && current_vec.r < m_vertices.size()) {

			current_vec.g = (current_vec.r - 2) * (current_vec.r != 0);
			current_vec.a = getValue(it.m_data, offset); // current value
			vec4 coords = coord_transform * (current_vec - MIN_VEC); // transformed coordinates

			m_vertices[(int) current_vec.r].position = m_origin + sf::Vector2f(coords.g, m_HEIGHT - coords.b); // index . pos = ~~~
			m_vertices[(int) current_vec.r].color = m_PLOT_COLOR; // index . color = ~~~
			m_vertices[(int) current_vec.r + 1].position = m_origin + sf::Vector2f(coords.r, m_HEIGHT - coords.a); // index + 1 . pos ~~~
			m_vertices[(int) current_vec.r + 1].color = m_PLOT_COLOR; // index + 1 . color  = ~~~

			current_vec.r += 2; // update index
			current_vec.b = current_vec.a; // update prev
			it++;
		}

		m_POINTS_TO_RENDER = (unsigned int) current_vec.r;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(bg, states);
		target.draw(&m_vertices[0], m_POINTS_TO_RENDER, sf::Lines, states);
	}
};


