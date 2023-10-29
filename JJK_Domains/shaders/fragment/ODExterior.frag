#version 330 core

uniform vec2 center;
uniform vec4 line_color;
uniform vec3 standard[4];
uniform vec2 resolution;
uniform float radius;
uniform float thresh_in = 0.f;

float THRESH_NORM = radius * 0.1f;
vec4 TRANSPARENT = vec4(0.f, 0.f, 0.f, 0.f);

float distanceFromLine(vec3 line_coefs, vec2 fragcoord) {
    float numerator = abs(line_coefs.x * fragcoord.x + line_coefs.y * fragcoord.y + line_coefs.z);
    float denominator = sqrt(line_coefs.x * line_coefs.x + line_coefs.y * line_coefs.y);
    return numerator / denominator;
}

void main() {    
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    bool frag_on_lines = false;
    bool THRESH_SET = thresh_in != 0.f;

    float BOUNDARY = float(THRESH_SET) * thresh_in + float(!THRESH_SET) * THRESH_NORM;
    for (int i = 0; i < 4; i++) {
        bool on_this_line = (isinf(standard[i].x)) ? abs(frag_coord.x - center.x) <= BOUNDARY : distanceFromLine(standard[i], frag_coord) <= BOUNDARY;
        frag_on_lines = frag_on_lines || on_this_line;
    }
    gl_FragColor = vec4(line_color.x, line_color.y, line_color.z, line_color.w * float(!frag_on_lines));

}
