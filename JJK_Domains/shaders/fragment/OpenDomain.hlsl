#version 330 core

uniform vec2 center;
uniform float radius;
uniform float thickness;
uniform vec4 line_color;
uniform vec3 standard;
uniform vec2 resolution;

float distanceFromCenter(vec4 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

float distanceFromLine(vec3 line_coefs, vec2 fragcoord) {
    float numerator = abs(line_coefs.x * fragcoord.x + line_coefs.y * fragcoord.y + line_coefs.z);
    float denom = distanceFromCenter(vec4(line_coefs.x, line_coefs.y, 0.f, 0.f), vec2(0.f, 0.f));
    return numerator / denom;
}

void main() {    
    float threshold = radius * .1f;
    float thresh_2 = radius * .1f;
    float distance = distanceFromCenter(gl_FragCoord, center);
    if (distance < radius) {
        gl_FragColor = vec4(0.f, 0.f, 0.f, 0.f);
        //return;
    }
    
    //vec2 origin = center - vec2(radius, radius);
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    float d = distanceFromLine(standard, frag_coord);
    
    if (d <= thresh_2) {
        gl_FragColor = vec4(0.f, 0.f, 1.f, 1.f);
        return;
    }
    
    gl_FragColor = line_color;
}