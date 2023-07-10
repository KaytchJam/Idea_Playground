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
		sf::Vector2f position = sf::Vector2f(0, 0);
	};

	extern struct keystates user_keys;
	extern struct mousestates user_mouse;
}
