#version 330 core

uniform vec2 center;
uniform float radius;
uniform float thickness;
uniform vec4 line_color;
uniform vec3 standard;

float distanceFromCenter(vec4 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

float distanceFromLine(vec3 line_coefs, vec4 fragcoord, vec2 origin, float rad) {
    float numerator = abs(line_coefs.x * (fragcoord.x - origin.x - rad) + line_coefs.y * (fragcoord.y - origin.y - rad) + line_coefs.z);
    float denom = distanceFromCenter(fragcoord, vec2(0.f, 0.f));
    return numerator / denom;
}

void main() {    
    float threshold = radius * .1f;
    float distance = distanceFromCenter(gl_FragCoord, center);
    if (distance < radius) {
        gl_FragColor = vec4(0.f, 0.f, 0.f, 0.f);
        return;
    }
    
    vec2 origin = center - vec2(radius, radius);
    
    if (distanceFromLine(standard, gl_FragCoord, origin, radius) <= threshold) {
        gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        return;
    }
    
    gl_FragColor = line_color;
}