#pragma once

#define _USE_MATH_DEFINES 
#include<cmath>

typedef float vec_t;

vec_t radians(vec_t degrees);
vec_t degrees(vec_t radians);
vec_t mod(vec_t x, vec_t m);

// Двухмерное поле действительных чисел
struct point2f {
	vec_t x, y;
	vec_t& operator[](int k) {
		switch (k) {
		case 0: return x;
		case 1: return y;
		default: throw "Index Error";
			break;
		}
	}

	friend point2f operator* (const vec_t& number, const point2f& p);
	friend point2f operator* (const point2f& p, const vec_t& number);
	friend vec_t operator* (const point2f& p1, const point2f& p2);
	friend point2f operator+ (const point2f& p1, const point2f& p2);
	friend point2f operator- (const point2f& p1, const point2f& p2);
	friend point2f operator- (const point2f& p);
	point2f operator+=(const point2f& p);
	point2f operator-=(const point2f& p);
	point2f operator*=(const vec_t& number) { return *this = *this * number; }
	bool    operator==(const point2f& p) { return (x == p.x) && (y == p.y); }
	bool    operator!=(const point2f& p) { return (x != p.x) || (y != p.y); }
};


// Трехмерное поле действительных чисел

struct vec3_t {
	vec_t x, y, z;
	vec_t& operator[](int k) {
		switch (k) {
		case 0: return x;
		case 1: return y;
		case 2: return z;

		default: throw "Index Error";
			break;
		}
	}

	vec_t operator[](int k) const {
		switch (k) {
		case 0: return x;
		case 1: return y;
		case 2: return z;

		default: throw "Index Error";
			break;
		}
	}

	friend vec3_t operator* (const vec_t& number, const vec3_t& p);
	friend vec3_t operator* (const vec3_t& p, const vec_t& number);
	friend vec_t  operator* (const vec3_t& p1, const vec3_t& p2);
	friend vec3_t operator+ (const vec3_t& p1, const vec3_t& p2);
	friend vec3_t operator- (const vec3_t& p1, const vec3_t& p2);
	friend vec3_t operator- (const vec3_t& p);
	vec3_t		  operator+=(const vec3_t& p);
	vec3_t		  operator-=(const vec3_t& p);
	vec3_t		  operator*=(const vec_t& number) { return *this = *this * number; }
	bool		  operator==(const vec3_t& p) const { return (x == p.x) && (y == p.y) && (z == p.z); }
	bool		  operator!=(const vec3_t& p) const { return (x != p.x) || (y != p.y) || (z != p.z); }
	vec_t length() { return sqrt(x * x + y * y + z * z); }
};

extern short sgn(vec_t x, vec_t accuracy = 0);
extern vec3_t normalize(const vec3_t& vector);
extern vec3_t Cross(const vec3_t& a, const vec3_t& b);

#define DotProduct(a, b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])

struct plane_s {
	vec3_t normal;
	vec_t   dist;
	bool operator==(const plane_s& other) const { return (normal == other.normal) and (dist == other.dist); }
	bool operator!=(const plane_s& other) const { return (normal != other.normal) or (dist != other.dist); }
	plane_s operator-() const { return { -normal, -dist }; }
	vec_t DistToPlane(vec3_t p) const { return normal * p - dist; }
};