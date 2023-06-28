#pragma once

struct keystates {
	bool RIGHT_HELD = false;
	bool LEFT_HELD = false;
	bool UP_HELD = false;
	bool DOWN_HELD = false;
};

struct mousestates {
	bool MOUSE_HELD = false;
};

extern struct keystates key_states;
extern struct mousestates mouse_states;