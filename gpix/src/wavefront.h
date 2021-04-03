#ifndef __WAVEFRONT_H__
#define __WAVEFRONT_H__

#include "util.h"

#include <boost/algorithm/string/predicate.hpp>

#include <vector>

struct face_t {
    double3 v[3];
};

struct wavefront_t {
    std::vector<double3> v;
    std::vector<face_t> f;

    void init(const char* filename) {
        auto file = fopen(filename, "r");
        char line[256];
        while(!feof(file)) {
            fgets(line, sizeof(line), file);
            if(boost::starts_with(line, "v ")) {
                double x, y, z;
                sscanf(line, "v %lf %lf %lf", &x, &y, &z);
                v.push_back({ x, y, z });
            }
            if(boost::starts_with(line, "f ")) {
                int i, j, k;
                sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &i, &j, &k);
                f.push_back({ v[i-1], v[j-1], v[k-1] });
            }
        }
    }
};

#endif // __WAVEFRONT_H__
