#pragma once
#include <SFML/Graphics.hpp>

// our basic SortAnimation object
class SortAnim {
	SortAnim() {}
	virtual ~SortAnim() {}
	virtual void onUpdate() {}
	virtual void onRender() {}
};