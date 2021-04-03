#ifndef __UTIL_H__
#define __UTIL_H__

#include <cmath>
#include <cstdint>

struct color_t {
    uint8_t r = 0, g = 0, b = 0, a = 0;
};

struct int2 {
    int x, y;
};

struct int3 {
    int x, y, z;
};

struct double2 {
    double x, y;
};

struct double3 {
    double x, y, z;
};



inline
int3 make_int3(const double3& a) {
    return { (int)a.x, (int)a.y, (int)a.z };
}

inline
double3 make_double3(const int3& a) {
    return { (double)a.x, (double)a.y, (double)a.z };
}



inline
bool operator == (const int2& a, const int2& b) {
    return a.x == b.x && a.y == b.y;
}

inline
bool operator == (const int3& a, const int3& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}



inline
double2 operator + (const double2& a, const double2& b) {
    return { a.x + b.x, a.y + b.y };
}

inline
double2 operator - (const double2& a, const double2& b) {
    return { a.x - b.x, a.y - b.y };
}

inline
double2 operator + (const double2& a, double b) {
    return { a.x + b, a.y + b };
}

inline
double2 operator - (const double2& a, double b) {
    return { a.x - b, a.y - b };
}

inline
double2 operator * (const double2& a, double b) {
    return { a.x * b, a.y * b };
}

inline
double2 operator / (const double2& a, double b) {
    return { a.x / b, a.y / b };
}

inline
double3 operator + (const double3& a, const double3& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline
double3 operator - (const double3& a, const double3& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline
double3 operator + (const double3& a, double b) {
    return { a.x + b, a.y + b, a.z + b };
}

inline
double3 operator - (const double3& a, double b) {
    return { a.x - b, a.y - b, a.z - b };
}

inline
double3 operator * (const double3& a, double b) {
    return { a.x * b, a.y * b, a.z * b };
}

inline
double3 operator / (const double3& a, double b) {
    return { a.x / b, a.y / b, a.z / b };
}



inline
double dot(const double2& a, const double2& b) {
    return a.x * b.x + a.y * b.y;
}

inline
double dot(const double3& a, const double3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline
double3 cross(const double3& a, const double3& b) {
    return { a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x };
}

inline
double length(const double2& a) {
    return std::sqrt(dot(a, a));
}

inline
double length(const double3& a) {
    return std::sqrt(dot(a, a));
}

inline
double2 unit(const double2& a) {
    return a / length(a);
}

inline
double3 unit(const double3& a) {
    return a / length(a);
}

inline
double2 lerp(const double2& a, const double2& b, double s) {
    return a + (b - a) * s;
}

#endif // __UTIL_H__
