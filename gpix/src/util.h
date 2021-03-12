#ifndef __UTIL_H__
#define __UTIL_H__

#include <cmath>
#include <cstdint>

struct color_t {
    uint8_t r = 0, g = 0, b = 0, a = 0;
};

struct int2_t {
    int x, y;

    bool operator == (const int2_t& v) const {
        return x == v.x && y == v.y;
    }
};

struct int3_t {
    int x, y, z;

    bool operator == (const int3_t& v) const {
        return x == v.x && y == v.y && z == v.z;
    }
};

struct double2_t {
    double x, y;

    double2_t operator + (const double2_t& v) const {
        return { x + v.x, y + v.y };
    }

    double2_t operator - (const double2_t& v) const {
        return { x - v.x, y - v.y };
    }

    double2_t operator * (double v) const {
        return { x * v, y * v };
    }

    double2_t operator / (double v) const {
        return { x / v, y / v };
    }
};

struct double3_t {
    double x, y, z;

    double dot(const double3_t& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    double3_t cross(const double3_t& v) const {
        return { y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x };
    }

    double3_t norm() const {
        double r = std::sqrt(x*x + y*y + z*z);
        return { x / r, y / r, z / r };
    }

    double3_t operator + (const double3_t& v) const {
        return { x + v.x, y + v.y, z + v.z };
    }

    double3_t operator - (const double3_t& v) const {
        return { x - v.x, y - v.y, z - v.z };
    }

    double3_t operator * (double v) const {
        return { x * v, y * v, z * v };
    }

    double3_t operator / (double v) const {
        return { x / v, y / v, z / v };
    }
};

#endif // __UTIL_H__
