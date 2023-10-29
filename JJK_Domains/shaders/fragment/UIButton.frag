#version 330 core

uniform vec2 topleft;
uniform vec2 botright;
uniform float radius = 0.f;
uniform vec4 button_color;

vec2 radVec = vec2(radius, -1 * radius);

bool is_point_at_border(in vec4 coord, in vec2 tls, in vec2 brs) {
	if (coord.x < tls.x || coord.x > brs.x) {
		if (coord.y > tls.y || coord.y < brs.y) { return true; }
	}
	return false;
}

bool easyCheck(in vec4 coord, in vec2 brs) {
	
}

float distSquared(in vec2 v1, in vec2 v2) {
	return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2);
}

bool beyondBorder(in vec4 coord, in vec2 tl, in vec2 br, float rad) {
	float yDiff = abs(br.y - tl.y);
	// float xDiff = abs(br.x - tl.x);
	float rSquared = pow(rad, 2);

	vec2 points[4] = { tl + vec2(rad, -1 * rad), tl + vec2(rad, -1 * yDiff - rad), br - vec2(rad, -1 * rad), br - vec2(rad, -1 * yDiff + rad)};
	for (int i = 0; i < 4; i++) { 
		if (distSquared(points[i], vec2(coord.x, coord.y)) > rSquared) { return true; }
	 }
	return false;
}

void main() {
	gl_FragColor = button_color;
	vec2 tlShift = topleft + radVec;
	vec2 brShift = botright - radVec;

	float rad = max(radius, 0.f);
	if (rad != 0) {
		// rounded edge logic
		if (is_point_at_border(gl_FragCoord, tlShift, brShift)) {
			gl_FragColor = vec4(button_color.r, button_color.g, button_color.b, !beyondBorder(gl_FragCoord, topleft, botright, rad));
		}
	}
}