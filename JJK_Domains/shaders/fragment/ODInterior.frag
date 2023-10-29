#version 330 core

uniform vec2 resolution;
uniform vec2 center;
uniform vec4 line_color;
uniform float radius;
uniform bool TUG_OF_WAR = false;

vec4 TRANSPARENT = vec4(0.f, 0.f, 0.f, 0.f);

float distanceFromCenter(vec2 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

void main() {    
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    bool inRange = distanceFromCenter(frag_coord, center) < radius;
    gl_FragColor = vec4(line_color.x, line_color.y, line_color.z, float(inRange) * 0.2f * float(!TUG_OF_WAR));
}