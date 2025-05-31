#include "mathlib.h"

point2f operator*(const vec_t& number, const point2f& p) { return { p.x * number, p.y * number }; }
point2f operator*(const point2f& p, const vec_t& number) { return { p.x * number, p.y * number }; }
vec_t   operator*(const point2f& p1, const point2f& p2) { return   p1.x * p2.x + p1.y * p2.y; }
point2f operator+(const point2f& p1, const point2f& p2) { return { p1.x + p2.x,  p1.y + p2.y }; }
point2f operator-(const point2f& p1, const point2f& p2) { return { p1.x - p2.x,  p1.y - p2.y }; }
point2f operator-(const point2f& p) { return { -p.x, -p.y }; }
point2f point2f::operator+=(const point2f& p) {
    this->x += p.x; this->y += p.y;
    return *this;
}
point2f point2f::operator-=(const point2f& p) {
    this->x -= p.x; this->y -= p.y;
    return *this;
}


vec3_t operator*(const vec_t & number, const vec3_t& p    ) { return { p.x * number, p.y * number, p.z * number }; }
vec3_t operator*(const vec3_t& p ,     const vec_t& number) { return { p.x * number, p.y * number, p.z * number }; }
vec_t  operator*(const vec3_t& p1,     const vec3_t& p2   ) { return   p1.x * p2.x + p1.y * p2.y + p1.z * p2.z; }
vec3_t operator+(const vec3_t& p1,     const vec3_t& p2   ) { return { p1.x + p2.x,  p1.y + p2.y,  p1.z + p2.z }; }
vec3_t operator-(const vec3_t& p1,     const vec3_t& p2   ) { return { p1.x - p2.x,  p1.y - p2.y,  p1.z - p2.z }; }
vec3_t operator-(const vec3_t& p) { return { -p.x,         -p.y,         -p.z }; }
vec3_t vec3_t::operator+=(const vec3_t& p) {
    this->x += p.x; this->y += p.y; this->z += p.z;
    return *this;
}
vec3_t vec3_t::operator-=(const vec3_t& p) {
    this->x -= p.x; this->y -= p.y; this->z -= p.z;
    return *this;
}

short sgn(vec_t x, vec_t accuracy) {
    if (abs(x) > accuracy) return (x > 0) ? (1) : (-1);
    return 0;
}

vec3_t normalize(const vec3_t& vector) {
    vec_t SqrLength = vector * vector;
    if (!SqrLength) 
        return vector;
    return vector * (1 / sqrt(SqrLength));
}

vec3_t Cross(const vec3_t& a, const vec3_t& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

vec_t radians(vec_t degrees) { return degrees * M_PI / 180; }
vec_t degrees(vec_t radians) { return radians * 180 / M_PI; }
vec_t mod(vec_t x, vec_t m) {
    x = fmod(x, m);
    return x < 0 ? x + m : x;
}

plane_s operator-(const plane_s& other) { return { -other.normal, -other.dist }; }