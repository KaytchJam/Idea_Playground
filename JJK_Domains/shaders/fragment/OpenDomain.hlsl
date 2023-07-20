#version 330 core

uniform vec2 center;
uniform float radius;
uniform float thickness;
uniform vec4 line_color;
uniform vec3 standard;
uniform vec2 resolution;

float threshold = radius * .1f;

float distanceFromCenter(vec4 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

float distanceFromLine(vec3 line_coefs, vec2 fragcoord) {
    float numerator = abs(line_coefs.x * fragcoord.x + line_coefs.y * fragcoord.y + line_coefs.z);
    float denom = sqrt(line_coefs.x * line_coefs.x + line_coefs.y + line_coefs.y);
    return numerator / denom;
}

void main() {    
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    
    if (distanceFromCenter(gl_FragCoord, center) < radius || distanceFromLine(standard, frag_coord) <= threshold) {
        gl_FragColor = vec4(0.f, 0.f, 0.f, 0.f);
        return;
    }
    
    gl_FragColor = line_color;
}