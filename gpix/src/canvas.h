#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "util.h"

#include <cassert>
#include <climits>
//#include <cmath>
#include <cstdio>

#include <algorithm>

struct canvas_t {
    int w;
    int h;

    color_t* pixels;
    int* zbuffer;

    canvas_t(int w, int h) : w(w), h(h) {
        pixels = new color_t[w*h];
        zbuffer = new int[w*h];
    }

    void clear() {
        for(int i = 0; i < w*h; i++) {
            pixels[i] = { 0, 0, 0, 0 };
            zbuffer[i] = INT_MIN;
        }
    }

    void draw(const int3_t& p, const color_t& color) {
        if(!(0 <= p.x && p.x < w) || !(0 <= p.y && p.y < h)) {
            printf("W [] draw: (%d, %d) out of bounds\n", p.x, p.y);
            return;
        }
        int i = w * p.y + p.x;
        if(p.z >= zbuffer[i]) {
            zbuffer[i] = p.z;
            pixels[i] = color;
        }
    }

    /**
     * <http://members.chello.at/~easyfilter/bresenham.html>
     */
    void line(int3_t p0, int3_t p1, const color_t& color) {
        int dx = std::abs(p1.x - p0.x), sx = p0.x < p1.x ? +1 : -1;
        int dy = std::abs(p1.y - p0.y), sy = p0.y < p1.y ? +1 : -1;
        int dz = std::abs(p1.z - p0.z), sz = p0.z < p1.z ? +1 : -1;
        int dm = std::max(std::max(dx, dy), dz), i = dm;
        p1 = { dm/2, dm/2, dm/2 };

        while(true) {
            draw(p0, color);
            if (i-- == 0) break;
            p1.x -= dx; if (p1.x < 0) { p1.x += dm; p0.x += sx; }
            p1.y -= dy; if (p1.y < 0) { p1.y += dm; p0.y += sy; }
            p1.z -= dz; if (p1.z < 0) { p1.z += dm; p0.z += sz; }
        }
    }

    void raster2(const int3_t& p0, int3_t p1l, int3_t p1r, const color_t& color) {
        assert(p1l.y == p1r.y);
        if(p1l.x > p1r.x) std::swap(p1l, p1r);
        int y = p1l.y, sy = p0.y < p1l.y ? +1 : -1;
        int xl = p1l.x, zl = p1l.z;
        int xr = p1r.x, zr = p1r.z;
        while(1) {
            for(int x = xl; x <= xr; x++) {
                int z = zl; if(xl != xr) z += (zr - zl) * (x - xl) / (xr - xl);
                draw({x, y, z}, color);
            }
            if(y == p0.y) break;

            y -= sy;
            double r = double(y - p0.y) / (p1l.y - p0.y);
            xl = p0.x + (p1l.x - p0.x) * r;
            xr = p0.x + (p1r.x - p0.x) * r;
            zl = p0.z + (p1l.z - p0.z) * r;
            zr = p0.z + (p1r.z - p0.z) * r;
        }
    }

    void raster(int3_t p0, int3_t p1, int3_t p2, const color_t& color) {
        if(p0.y > p2.y) std::swap(p0, p2);
        // y0 <= y2
        if(p0.y > p1.y) std::swap(p0, p1);
        // y0 <= y1, y2
        if(p1.y > p2.y) std::swap(p1, p2);
        // y0 <= y1 <= y2
        if(p1.y == p2.y) {
            raster2(p0, p1, p2, color);
            return;
        }
        // y0 <= y1 < y2
        int xm = p0.x + (p2.x - p0.x) * double(p1.y - p0.y) / (p2.y - p0.y);
        int zm = p0.z + (p2.z - p0.z) * double(p1.y - p0.y) / (p2.y - p0.y);
        raster2(p0, p1, {xm,p1.y,zm}, color);
        raster2(p2, p1, {xm,p1.y,zm}, color);
    }

};

#endif // __CANVAS_H__
