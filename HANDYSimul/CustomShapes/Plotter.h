#pragma once
#include "SFML/Graphics.hpp"
#include "../MyTools/RingBuffer.h"
#include "../MyTools/MyV.h"
#include "../MyTools/PointerVector.h"\

#define THICKNESS 3.f
#define THICKNESS_VECTOR sf::Vector2f(0, THICKNESS)
#define VERTEX_OFFSET 4

class Plotter : public sf::Drawable {
private:
	unsigned int m_LENGTH;
	unsigned int m_HEIGHT;
	unsigned int m_POINTS_TO_RENDER;

	sf::Vector2f m_origin;
	sf::Color m_PLOT_COLOR;

	sf::RectangleShape* bg;
	std::vector<sf::Vertex> m_vertices;
	
public:
	Plotter(sf::Vector2f size, unsigned int NUMBER_OF_POINTS, sf::Color line_color = sf::Color::Black, sf::Color bg_color = sf::Color::White) 
		: m_LENGTH((int) size.x), m_HEIGHT((int) size.y), m_POINTS_TO_RENDER(0) {
		m_vertices = std::vector<sf::Vertex>(NUMBER_OF_POINTS * VERTEX_OFFSET);
		m_PLOT_COLOR = line_color;
		bg = new sf::RectangleShape(size + THICKNESS_VECTOR);
		bg->setFillColor(bg_color);
	}

	~Plotter() {}

	Plotter& setPosition(const sf::Vector2f& pos) {
		m_origin = pos;
		bg->setPosition(pos - THICKNESS_VECTOR);
		return *this;
	}

	Plotter& move(const sf::Vector2f& shift) {
		m_origin += shift;
		bg->setPosition(bg->getPosition() - THICKNESS_VECTOR + shift);
		return *this;
	}

	Plotter& setColor(const sf::Color& kalah) {
		m_PLOT_COLOR = kalah;
		return *this;
	}

	Plotter& setBackgroundColor(const sf::Color& kalah) {
		bg->setFillColor(kalah);
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
			coord_transform = diag(map(diagonal_inverse_vec, [](float f) {return 1.f / (f + 0.001f * (f == 0.0f));  })); // diag(vector^-1)
		}

		vec4 current_vec = { 0 /*prev index*/, 1 /*index + 1*/, getValue(rb.front(), offset) /*prev*/, 0 /*current*/ };

		RingBuffer<vec4>::iterator it = rb.begin();
		while (it != rb.end() && current_vec.r < m_vertices.size()) {

			current_vec.g = (current_vec.r - VERTEX_OFFSET) * (current_vec.r != 0);
			current_vec.a = getValue(it.m_data, offset); // current value
			vec4 coords = coord_transform * (current_vec - MIN_VEC); // transformed coordinates

			const sf::Vector2f OLD_MIDPOINT = m_origin + sf::Vector2f(coords.g, m_HEIGHT - coords.b);
			const sf::Vector2f NEW_MIDPOINT = m_origin + sf::Vector2f(coords.r, m_HEIGHT - coords.a);

			m_vertices[(size_t)current_vec.r].position = { OLD_MIDPOINT.x, m_origin.y + m_HEIGHT};//OLD_MIDPOINT + THICKNESS_VECTOR;// index . pos = ~~~
			m_vertices[(size_t)current_vec.r + 1].position = OLD_MIDPOINT - THICKNESS_VECTOR;
			m_vertices[(size_t)current_vec.r].color = m_PLOT_COLOR; // index . color = ~~~
			m_vertices[(size_t)current_vec.r + 1].color = m_PLOT_COLOR;

			m_vertices[(size_t)current_vec.r + 2].position = NEW_MIDPOINT - THICKNESS_VECTOR; // index + 1 . pos ~~~
			m_vertices[(size_t)current_vec.r + 3].position = { NEW_MIDPOINT.x, m_origin.y + m_HEIGHT }; //NEW_MIDPOINT + THICKNESS_VECTOR;
			m_vertices[(size_t)current_vec.r + 2].color = m_PLOT_COLOR; // index + 1 . color  = ~~~
			m_vertices[(size_t)current_vec.r + 3].color = m_PLOT_COLOR; // index + 1 . color  = ~~~

			current_vec.r += VERTEX_OFFSET; // update index
			current_vec.b = current_vec.a; // update prev
			it++;
		}

		m_POINTS_TO_RENDER = (unsigned int) current_vec.r;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(*bg, states);
		target.draw(&m_vertices[0], m_POINTS_TO_RENDER, sf::Quads, states);
	}

	static void format_row(PointerVector<sf::Drawable>& plotGroup, const sf::Vector2f REFERENCE_POSITION, const float SPACING, const size_t START, const size_t END) {
		for (size_t i = 0; i < END - START; i++) {
			Plotter* cur_plot = (Plotter*)plotGroup.get(i + START);
			const sf::Vector2f cur_size = cur_plot->getSize();
			cur_plot->setPosition(REFERENCE_POSITION + sf::Vector2f(i * (SPACING + cur_size.x), 0));
		}
	}

	static void format_col(PointerVector<Plotter>& plotGroup, const sf::Vector2f REFERENCE_POSITION, const float SPACING, const size_t START, const size_t END) {
		for (size_t i = 0; i < END - START; i++) {
			Plotter* cur_plot = plotGroup.get(i);
			const sf::Vector2f cur_size = cur_plot->getSize();
			cur_plot->setPosition(REFERENCE_POSITION + sf::Vector2f(0, i * (SPACING + cur_size.y)));
		}
	}

	static void format_window(PointerVector<sf::Drawable>& plotGroup, sf::Vector2f REFERENCE_POSITION, const float SPACING_VERTICAL, const float SPACING_HORIZONTAL, const size_t NUM_ROWS) {
		size_t DIVS = plotGroup.size() / NUM_ROWS;
		size_t START = 0;
		for (int r = 0; r < NUM_ROWS; r++) {
			format_row(plotGroup, REFERENCE_POSITION + sf::Vector2f(0, r * (SPACING_VERTICAL + ((Plotter*)plotGroup.get(0))->getSize().y)), SPACING_HORIZONTAL, START, START + DIVS);
			START += DIVS;
		}
	}

};
