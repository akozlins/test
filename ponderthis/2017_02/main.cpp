/**
 * https://www.research.ibm.com/haifa/ponderthis/challenges/February2017.html
 */

#include <stdint.h>
#include <stdio.h>

int r1(int s, int v, int n) {
    int r = 0;
    for(int i = 0; i < n; i++) {
        int b = v & 1; v >>= 1;
        if(s % 7 == b) r = r + 2 * b - 1;
        s = 6 + s * (5 + s * s * s * (2 + s * (3 + s)))
          + b * (5 + s *(5 + s * (6 + s * s * (3 + s * 6))));
        s %= 7;
    }
    return r;
}


int t[7][2];

int r2(int s_, int v, int n) {
    int s = s_;
    int r = 0;
    for(int i = 0; i < n; i++) {
        int b = v & 1; v >>= 1;
        if(s == b) r = r + 2 * b - 1;
        s = t[s][b];
    }
    return r;
}

uint64_t n_sr[7][16];
uint64_t n_ssr[7][7][16];

int r3(int s_, int v, int n) {
    int s = s_;
    int r = 0;
    for(int i = 0; i < n; i++) {
        int b = v & 1; v >>= 1;
        if(s == b) r = r + 2 * b - 1;
        s = t[s][b];
    }
    n_sr[s_][8+r]++;
    n_ssr[s_][s][8+r]++;
    return r;
}

int main() {
    for(int s = 0; s < 7; s++) {
        int s2 = s * s;
        int s3 = s2 * s;
        int i = 6 + s * (5 + s3 * (2 + s * (3 + s)));
        int j = 5 + s * (5 + s * (6 + s2 * (3 + 6 * s)));
        i = i % 7; j = (i + j) % 7;
        t[s][0] = i; t[s][1] = j;
        printf("t[%d] = { %d, %d }\n", s, i, j);
    }

    for(int n = 6, i = 0, v = 0; v < (1 << n); v++) {
        int r = r2(2, v, n);
        if(r != 0) printf("%d: %08X => %d\n", ++i, v, r);
    }

    for(int n = 21, v = 0; v < (1 << n); v++) {
        for(int s = 0; s <= 6; s++) {
            r3(s, v, n);
        }
    }
    uint64_t N = (uint64_t(1) << 42);
    for(int s = 0; s <= 6; s++) for(int r = -8; r < 8; r++) {
        if(n_sr[s][8+r] == 0) continue;
        printf("n_ssr[2][%d][%d] * n_sr[%d][%d] = %lu * %lu\n", s, r, s, -r, n_ssr[2][s][8+r], n_sr[s][8-r]);
        N -= n_ssr[2][s][8+r] * n_sr[s][8-r];
    }
    printf("N = %lu\n", N);

    return 0;
}
