#version 330 core

uniform vec2 center;
uniform float radius;
uniform float thickness;
uniform vec4 line_color;

float distanceFromCenter(vec4 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

void main() {    
    float distance = distanceFromCenter(gl_FragCoord, center);
    if (distance < radius) {
        gl_FragColor = vec4(0.f, 0.f, 0.f, 0.f);
        return;
    }
    float filler = thickness + 1;
    gl_FragColor = line_color;
}