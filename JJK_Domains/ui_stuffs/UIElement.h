#pragma once
#include <SFML/Graphics.hpp>

// Parent class for UI "Widgets"
// Please do not call "new UIElement()," call a subclass
class UIElement : public sf::Drawable {
protected:
	const void** m_args;
	const unsigned int m_NUM_ARGS;
public:
	UIElement(const void** p_args, const unsigned int p_NUM_ARGS) : m_args(p_args), m_NUM_ARGS(p_NUM_ARGS) {}
	virtual ~UIElement() {}

	virtual sf::Vector2f getPosition() { return sf::Vector2f(0.f, 0.f); }
	virtual void onUpdate(float deltaTime) {}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {}
};