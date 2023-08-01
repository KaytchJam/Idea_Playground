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
		bool MOUSE_HELD = false;
		bool LEFT_HELD = false;
		bool RIGHT_HELD = false;
		bool MOUSE_RELEASED = false;
		bool LEFT_RELEASED = false;
		bool RIGHT_RELEASED = false;
		bool HOLDING_OBJECT = false;
		sf::Vector2i position = sf::Vector2i(0, 0);
	};

	extern struct keystates user_keys;
	extern struct mousestates user_mouse;
}