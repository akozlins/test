
#include <cmath>

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

    double3_t operator - (const double3_t& v) const {
        return { x - v.x, y - v.y, z - v.z };
    }
};
