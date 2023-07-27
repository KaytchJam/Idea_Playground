#version 330 core

uniform vec2 center;
uniform vec4 line_color;
//uniform vec3 standard; // vector of standard form coefficients for a line
uniform vec3 standard[4];
uniform vec2 resolution;
uniform float radius;
uniform float thickness;

uniform bool TUG_OF_WAR = false;

float THRESHOLD = radius * .1f;
vec4 TRANSPARENT = vec4(0.f, 0.f, 0.f, 0.f);

float distanceFromCenter(vec4 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

float distanceFromLine(vec3 line_coefs, vec2 fragcoord) {
    float numerator = abs(line_coefs.x * fragcoord.x + line_coefs.y * fragcoord.y + line_coefs.z);
    float denominator = sqrt(line_coefs.x * line_coefs.x + line_coefs.y + line_coefs.y);
    return numerator / denominator;
}

void main() {    
    
    if (distanceFromCenter(gl_FragCoord, center) < radius) {
        if (TUG_OF_WAR) gl_FragColor = TRANSPARENT;
        else gl_FragColor = vec4(line_color.x, line_color.y, line_color.z, 0.2f);
        return;
    }
    
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    for (int i = 0; i < 4; i++) {
        if (distanceFromLine(standard[i], frag_coord) <= THRESHOLD) {
            gl_FragColor = TRANSPARENT;
            return;
        }
    }
    
    gl_FragColor = line_color;
}