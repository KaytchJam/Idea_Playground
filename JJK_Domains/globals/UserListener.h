#pragma once
#include "SFML/Graphics.hpp"

namespace kay {
	struct keystates {
		bool RIGHT_HELD = false;
		bool LEFT_HELD = false;
		bool UP_HELD = false;
		bool DOWN_HELD = false;
	};

	struct mousestates {
		// general states
		bool MOUSE_HELD = false;								// is any mouse button being held
		bool MOUSE_RELEASED = false;							// has any mouse button been released
		bool HOLDING_OBJECT = false;							// whether the mouse is 'holding' something or not
		sf::Vector2i position = sf::Vector2i(0, 0);				// current mouse position

		// left states
		bool LEFT_HELD = false;									// is the left mouse button being held
		bool LEFT_RELEASED = false;								// has the left mouse button been released
		sf::Vector2i LEFT_CLICK_POSITION = sf::Vector2i(0, 0);	// position of the last left 'click'

		// right states
		bool RIGHT_HELD = false;								// is the right mouse button being held
		bool RIGHT_RELEASED = false;							// has the right mouse button been released
		sf::Vector2i RIGHT_CLICK_POSITION = sf::Vector2i(0, 0); // position of the last right 'click'
	};

	extern struct keystates user_keys;
	extern struct mousestates user_mouse;
}