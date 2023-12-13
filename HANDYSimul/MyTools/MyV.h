#pragma once
#include <iostream>
#include <cmath>

namespace lalg {
    //
    // VECTORS
    //
    struct vec4 { float r, g, b, a; };
    struct ivec4 { int r, g, b, a; };

    // type conversion
    vec4 to_vec4(const ivec4& v) { return { (float)v.r, (float)v.g, (float)v.b, (float)v.a }; }
    ivec4 to_ivec4(const vec4& v) { return { (int)v.r, (int)v.g, (int)v.b, (int)v.a }; }

    // vector operations
    vec4 operator+(const vec4& v1, const vec4& v2) { return { v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a }; }
    vec4 operator-(const vec4& v1, const vec4& v2) { return { v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a }; }
    vec4 operator*(const float scalar, const vec4& v) { return { scalar * v.r, scalar * v.g, scalar * v.b, scalar * v.a }; }
    vec4 operator*(const vec4& v, const float scalar) { return scalar * v; }
    vec4 operator/(const vec4& v, const float scalar) { return { v.r / scalar, v.g / scalar, v.b / scalar, v.a / scalar }; }
    vec4 operator/(const float scalar, const vec4& v) { return scalar * v; }

    // vector functions
    float magnitude(const vec4& v) { return sqrt(v.r * v.r + v.g * v.g + v.b * v.b + v.a * v.a); }
    vec4 cross(const vec4& v1, const vec4& v2) { return { v1.g * v2.b - v1.b * v2.g, v1.b * v2.r - v1.r * v2.b, v1.r * v2.g - v1.g * v2.r, 0 }; }
    vec4 normalize(const vec4& v) { return v / magnitude(v); }
    float dot(const vec4& v1, const vec4& v2) { return v1.r * v2.r + v1.g * v2.g + v1.b * v2.b + v1.a * v2.a; }
    void printVec(const vec4& v) { std::printf("[%f, %f, %f, %f]\n", v.r, v.g, v.b, v.a); }

    //
    // MATRICES
    //
    struct mat4 { vec4 r1, r2, r3, r4; };

    // get a 0-indexed column from mat4
    vec4 getColumn(const mat4& m, const uint8_t column) {
        if (column > 3) return { 0, 0, 0, 0 }; // need some placeholder error thing
        vec4* cVec = (vec4*)((float*)&m + column);
        return { cVec->r, (cVec + 1)->r, (cVec + 2)->r, (cVec + 3)->r };
    }

    // modify the passed in matrix, and set a 0-indexed column equal to the specifed vector
    void setColumn(const mat4& m, const vec4& v, const uint8_t column) {
        if (column <= 3) {
            vec4* cVec = (vec4*)((float*)&m + column);
            cVec->r = v.r;
            (cVec + 1)->r = v.g;
            (cVec + 2)->r = v.b;
            (cVec + 3)->r = v.a;
        }
    }

    // convert rows to columns
    mat4 transpose(const mat4& m) { return { getColumn(m,0), getColumn(m,1), getColumn(m,2), getColumn(m,3) }; }

    // returns a translation matrix (we're using the alpha channel as our homogenous coordinate)
    mat4 const translationMat(const float tx, const float ty, const float tz) {
        return {{1, 0, 0, tx},
                {0, 1, 0, ty},
                {0, 0, 1, tz},
                {0, 0, 0, 1}};
    }

    // returns a scaling matrix using the alpha channel as a homogenous coordinate
    mat4 const scalingMat(const float sx, const float sy, const float sz) {
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

    // rotation with respect to the x (or red) axis
    mat4 const rotationMatX(float theta) {
        return { {1,           0,                0, 0},
                {0, cosf(theta), -1 * sinf(theta), 0},
                {0, sinf(theta),      cosf(theta), 0},
                {0,           0,                0, 1} };
    }

    // rotation with respect to the y (or green) axis
    mat4 const rotationMatY(float theta) {
        return { {     cosf(theta), 0, sinf(theta), 0},
                {               0, 1,           0, 0},
                {-1 * sinf(theta), 0, cosf(theta), 0},
                {               0, 0,           0, 1} };
    }

    // rotation with respect to the z (or blue) axis
    mat4 const rotationMatZ(float theta) {
        return {{cosf(theta), -1 * sinf(theta), 0, 0},
                {sinf(theta),      cosf(theta), 0, 0},
                {          0,                0, 1, 0},
                {          0,                0, 0, 1} };
    }

    // matrix vector and matrix matrix operations
    vec4 operator*(const mat4& m, const vec4& v) { return { dot(m.r1, v), dot(m.r2, v), dot(m.r3, v), dot(m.r4, v) }; }
    mat4 operator+(const mat4& m1, const mat4& m2) { return { m1.r1 + m2.r1, m1.r2 + m2.r2, m1.r3 + m2.r3, m1.r4 + m2.r4 }; }
    mat4 operator*(const mat4& m1, const mat4& m2) {
        mat4 col_major = { m1 * getColumn(m2, 0), m1 * getColumn(m2, 1), m1 * getColumn(m2, 2), m1 * getColumn(m2, 3) };
        return transpose(col_major);
    }

    void printMat(const mat4& m) {
        printVec(m.r1);
        printVec(m.r2);
        printVec(m.r3);
        printVec(m.r4);
    }
}
