#pragma once
#include <iostream>
#include <cmath>
#include <functional>

namespace lalg {
    //
    // VECTORS
    //
    struct vec4 { float r, g, b, a; };
    struct ivec4 { int r, g, b, a; };

    // type conversion
    vec4 make_vec4(float f1, float f2, float f3, float f4) { return { f1, f2, f3, f4 }; }
    vec4 to_vec4(const ivec4& v) { return { (float)v.r, (float)v.g, (float)v.b, (float)v.a }; }
    ivec4 to_ivec4(const vec4& v) { return { (int)v.r, (int)v.g, (int)v.b, (int)v.a }; }

    // vector operations
    vec4 operator+(const vec4& v1, const vec4& v2) { return { v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a }; }
    vec4 operator-(const vec4& v1, const vec4& v2) { return { v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a }; }
    vec4 operator*(const float scalar, const vec4& v) { return { scalar * v.r, scalar * v.g, scalar * v.b, scalar * v.a }; }
    vec4 operator*(const vec4& v, const float scalar) { return scalar * v; }
    vec4 operator/(const vec4& v, const float scalar) { return { v.r / scalar, v.g / scalar, v.b / scalar, v.a / scalar }; }
    vec4 operator/(const float scalar, const vec4& v) { return scalar * v; }
    std::ostream& operator<<(std::ostream& os, const vec4& v) {
        return os << "[" << v.r << "," << v.g << "," << v.b << "," << v.a << "]";
    }

    // vector functions
    float get_value(const vec4& v, uint8_t offset) { return *((float*)&v + offset); }
    float magnitude(const vec4& v) { return sqrt(v.r * v.r + v.g * v.g + v.b * v.b + v.a * v.a); }
    float dot(const vec4& v1, const vec4& v2) { return v1.r * v2.r + v1.g * v2.g + v1.b * v2.b + v1.a * v2.a; }
    float distance(const vec4& v1, const vec4& v2) { return std::sqrtf(std::powf(v2.r - v1.r, 2) + std::powf(v2.g - v1.g, 2) + std::powf(v2.b - v1.b, 2) + std::powf(v2.a - v1.a, 2)); }
    vec4 cross(const vec4& v1, const vec4& v2) { return { v1.g * v2.b - v1.b * v2.g, v1.b * v2.r - v1.r * v2.b, v1.r * v2.g - v1.g * v2.r, 0 }; }
    vec4 normalize(const vec4& v) { return v / magnitude(v); }
    vec4 pow(const vec4& v, const float power) { return { powf(v.r, power), powf(v.g, power) , powf(v.b, power) , powf(v.a, power) }; }
    vec4 project(const vec4& v, const vec4& onto) { return onto * (dot(v, onto) / dot(onto, onto)); }
    void print_vec(const vec4& v) { std::printf("[%f, %f, %f, %f]\n", v.r, v.g, v.b, v.a); }

    void set_index(const vec4& v, const uint8_t index, const float value) {
        if (index > 3) {
            std::printf("bro please put an index in the inclusive range 0 - 3.\n");
            return;
        }

        float* vec_ptr = (float*) &v;
        *(vec_ptr + index) = value;
    }

    // special vectors
    vec4 const ones_vec() { return { 1, 1, 1, 1 }; }
    vec4 const zero_vec() { return { 0, 0, 0, 0 }; }
    vec4 const n_vec(float n) { return { n, n, n, n }; }

    //
    // MATRICES
    //
    struct mat4 { vec4 r1, r2, r3, r4; };

    // get a 0-indexed column from mat4
    vec4 get_column(const mat4& m, const uint8_t column) {
        if (column > 3) return { 0, 0, 0, 0 }; // need some placeholder error thing
        vec4* cVec = (vec4*)((float*)&m + column);
        return { cVec->r, (cVec + 1)->r, (cVec + 2)->r, (cVec + 3)->r };
    }

    // modify the passed in matrix, and set a 0-indexed column equal to the specifed vector
    void set_column(const mat4& m, const vec4& v, const uint8_t column) {
        if (column <= 3) {
            vec4* cVec = (vec4*)((float*)&m + column);
            cVec->r = v.r;
            (cVec + 1)->r = v.g;
            (cVec + 2)->r = v.b;
            (cVec + 3)->r = v.a;
        }
    }

    // convert rows to columns
    mat4 transpose(const mat4& m) { return { get_column(m,0), get_column(m,1), get_column(m,2), get_column(m,3) }; }

    // returns a translation matrix (we're using the alpha channel as our homogenous coordinate)
    mat4 const translation_mat(const float tx, const float ty, const float tz) {
        return {{1, 0, 0, tx},
                {0, 1, 0, ty},
                {0, 0, 1, tz},
                {0, 0, 0, 1}};
    }

    // returns a scaling matrix using the alpha channel as a homogenous coordinate
    mat4 const scaling_mat(const float sx, const float sy, const float sz) {
        return {{sx, 0,  0, 0},
                {0, sy,  0, 0},
                {0,  0, sz, 0},
                {0,  0,  0, 1}};
    }

    // returns the identity matrix
    mat4 const eye() {
        return {{1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}};
    }

    // returns a 4 * 4 matrix of ones
    mat4 const ones_mat() {
        return {{1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1},
                {1, 1, 1, 1}};
    }

    // returns a 4 * 4 matrix of zeroes
    mat4 const zero_mat() {
        return {{0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}};
    }

    // returns a 4 * 4 matrix of the value n
    mat4 const n_mat(float n) {
        return {{n, n, n, n},
                {n, n, n, n},
                {n, n, n, n},
                {n, n, n, n} };
    }

    // creates a matrix with values of vector v along the diagonal
    mat4 const diag(const vec4& v) {
        return {{v.r, 0, 0, 0},
                {0, v.g, 0, 0},
                {0, 0, v.b, 0},
                {0, 0, 0, v.a}};
    }

    // rotation with respect to the x (or red) axis
    mat4 const rotation_mat_x(const float theta) {
        return {{1,           0,                0, 0},
                {0, cosf(theta), -1 * sinf(theta), 0},
                {0, sinf(theta),      cosf(theta), 0},
                {0,           0,                0, 1} };
    }

    // rotation with respect to the y (or green) axis
    mat4 const rotation_mat_y(const float theta) {
        return { {    cosf(theta), 0, sinf(theta), 0},
                {               0, 1,           0, 0},
                {-1 * sinf(theta), 0, cosf(theta), 0},
                {               0, 0,           0, 1} };
    }

    // rotation with respect to the z (or blue) axis
    mat4 const rotation_mat_z(const float theta) {
        return {{cosf(theta), -1 * sinf(theta), 0, 0},
                {sinf(theta),      cosf(theta), 0, 0},
                {          0,                0, 1, 0},
                {          0,                0, 0, 1} };
    }

    // matrix vector operations
    vec4 operator*(const mat4& m, const vec4& v) { return { dot(m.r1, v), dot(m.r2, v), dot(m.r3, v), dot(m.r4, v) }; }

    // matrix scalar operations
    mat4 operator*(const float f, const mat4& m) { return { f * m.r1, f * m.r2, f * m.r3, f * m.r4 }; }
    mat4 operator*(const mat4& m, const float f) { return f * m; }
    mat4 operator/(const mat4& m, const float f) { return { m.r1 / f, m.r2 / f, m.r3 / f, m.r4 / f }; }
    mat4 operator/(const float f, const mat4 & m) { return f * m; }
    
    // matrix matrix operations
    mat4 operator+(const mat4& m1, const mat4& m2) { return { m1.r1 + m2.r1, m1.r2 + m2.r2, m1.r3 + m2.r3, m1.r4 + m2.r4 }; }
    mat4 operator*(const mat4& m1, const mat4& m2) {
        mat4 col_major = { m1 * get_column(m2, 0), m1 * get_column(m2, 1), m1 * get_column(m2, 2), m1 * get_column(m2, 3) };
        return transpose(col_major);
    }
    std::ostream& operator<<(std::ostream& os, const mat4& m) {
        return os << "\n[" << m.r1 << ",\n" << m.r2 << ",\n" << m.r3 << ",\n" << m.r4 << "]";
    }
    
    // apply some function func to all elements in the matrix and return a new matrix
    mat4 map(const mat4& m, float (*func)(float)) {
        mat4 out = zero_mat();
        float* target_ptr = (float*) & out;
        float* from_ptr = (float*)&m;

        for (int i = 0; i < 16; i++) {
            *target_ptr = func(*from_ptr);
            target_ptr++;
            from_ptr++;
        }

        return out;
    }

    vec4 map(const vec4& v, float (*func)(float)) {
        vec4 out = zero_vec();
        out.r = func(v.r);
        out.g = func(v.g);
        out.b = func(v.b);
        out.a = func(v.a);
        return out;
    }

    // apply some function func to all elements in the matrix, and update the matrix
    void map_mut(mat4* m, float(*func)(float)) {
        float* elem_ptr = (float*)m;
        for (int i = 0; i < 16; i++) {
            *elem_ptr = func(*elem_ptr);
            elem_ptr++;
        }
    }

    void map_mut(vec4* v, float(*func)(float)) {
        v->r = func(v->r);
        v->g = func(v->g);
        v->b = func(v->b);
        v->a = func(v->a);
    }

    vec4 map_capture(const vec4& v, std::function<float(float)> func) {
        vec4 out = zero_vec();
        out.r = func(v.r);
        out.g = func(v.g);
        out.b = func(v.b);
        out.a = func(v.a);
        return out;
    }

    // print da matrix
    void print_mat(const mat4& m) {
        print_vec(m.r1);
        print_vec(m.r2);
        print_vec(m.r3);
        print_vec(m.r4);
    }
    
    // normalize each element in vector v to a different range determined by min_vec and max_vec
    vec4 normalize_each_vec(const lalg::vec4& v, const lalg::vec4& min_vec, const lalg::vec4& max_vec) {
        return diag(map(max_vec - min_vec, [](float f) { return 1.f / (f + 0.001f * (f != 0.0f));  })) * (v - min_vec);
    }
}
