#version 330 core

// Window ~ View
uniform vec2 resolution;

// Circle Rendering
uniform vec2 center;
uniform float radius;
uniform vec4 line_color;
uniform bool TUG_OF_WAR = false;

// Line Rendering
uniform vec3 standard[4];
uniform float thresh_in = 0.f;

// euclidean distance function
float distanceFromCenter(vec2 fragcoord, vec2 center) {
    return sqrt(pow(fragcoord.x - center.x, 2) + pow(fragcoord.y - center.y, 2));
}

// minimum distance from point to standard form line
float distanceFromLine(vec3 line_coefs, vec2 fragcoord) {
    float numerator = abs(line_coefs.x * fragcoord.x + line_coefs.y * fragcoord.y + line_coefs.z);
    float denominator = sqrt(line_coefs.x * line_coefs.x + line_coefs.y * line_coefs.y);
    return numerator / denominator;
}

void main() {    
    vec2 frag_coord = vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y);
    bool inRange = distanceFromCenter(frag_coord, center) < radius;
    bool frag_on_lines = false;
    bool THRESH_SET = thresh_in != 0.f;
    float BOUNDARY = float(THRESH_SET) * thresh_in + float(!THRESH_SET) * radius * 0.1f;

    for (int line = 0; line < 4; line++) {
        vec3 line_coefficients = standard[line];
        bool on_this_line = (isinf(line_coefficients.x)) ? abs(frag_coord.x - center.x) <= BOUNDARY : distanceFromLine(line_coefficients, frag_coord) <= BOUNDARY;
        frag_on_lines = frag_on_lines || on_this_line;
    }

    gl_FragColor = vec4(line_color.x, line_color.y, line_color.z, float(inRange) * 0.2f * float(!TUG_OF_WAR) + float(!inRange) * float(!frag_on_lines) * line_color.w);
}