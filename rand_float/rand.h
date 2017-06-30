#ifndef _RAND_H
#define _RAND_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct rand_t {
    union u_t {
        int i;
        float f;
    };

    int b = 0;
    int i = 0;
    int i_max = 31;

    rand_t() {
        while((0x7FFFFFFF >> (31 - i_max)) > RAND_MAX) i_max -= 1;
        printf("RAND_MAX = 0x%08X => i_max = %d\n", RAND_MAX, i_max);
    }

    int next(int n) {
        assert(0 < n && n <= 31);

        int r = 0;
        while(true) {
            if(i == 0) {
                b = rand();
                i = i_max;
            }
            if(i >= n) {
                r <<= n;
                r += b & (0x7FFFFFFF >> (31 - n));
                b >>= n;
                i -= n;
                return r;
            }
            r <<= i;
            r += b & (0x7FFFFFFF >> (31 - i));
            n -= i;
            i = 0;
        }
    }

    float uniform_exp(int ea, int eb) {
        assert(0 <= ea && ea <= eb && eb < 255);

        start:
        int e = eb;
        while(e > 0 && next(1)) e--;
        if(e < ea) goto start;

        int m;
        if(e > 0) {
            m = next(23);
            m += next(1);
        }
        else {
            m = 0;
            e += next(1);
        }

        u_t u;
        u.i = (e << 23) + m;
        return u.f;
    }

    float uniform() {
        return uniform_exp(0, 126);
    }

    float uniform(float a) {
        if(a < 0) return -uniform(-a);

        u_t u;
        u.f = a;
        int e = u.i >> 23;
        if(e == 0) abort();
        int m = u.i & 0x7FFFFF;
        if(m == 0) return uniform_exp(0, e - 1);

        while(true) {
            if(next(1)) return uniform_exp(0, e - 1);
            int m_ = next(23);
            if(m_ < m) {
                u.i = (e << 23) + m_ + next(1);
                break;
            }
        }

        return u.f;
    }

    float uniform(float a, float b) {
        if(b < a) return uniform(b, a);
        if(a < 0 && b < -a) return -uniform(-b, -a);

        u_t ua, ub;
        ua.f = a; ub.f = b;
        int ea = (ua.i >> 23) & 0xFF, eb = (ub.i >> 23) & 0xFF;

        if(a < 0 && b > 0) {
            float r;
            do { r = uniform_exp(0, eb); if(next(1)) return r; } while(-r < a);
            return -r;
        }

        // TODO: ea == eb

        // a > 0 && b > 0
        float r;
        do { r = uniform_exp(ea, eb); } while(r < a || b < r);
        return r;
    }
};

#endif // _RAND_H
