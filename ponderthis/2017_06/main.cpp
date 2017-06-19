/**
 *  Created on: 2017.05.29
 *      Author: Alexandr Kozlinskiy
 *
 * https://www.research.ibm.com/haifa/ponderthis/challenges/June2017.html
 *
 * Ponder This Challenge:
 * This month's challenge involves a game that can be played with seven-digit numbers (leading
 * zeroes are allowed), such as the serial number of a bus ticket, a license plate number in Israel,
 * phone numbers, etc.
 * To play the game, take the seven-digit number and place the four basic operations (+, -, *,
 * and /) and parentheses between the digits, using each digit exactly once (the parentheses and
 * operations can be used more than once). Without changing the order or concatenating two or more
 * digits into a larger number, use the basic operations and parentheses so that the answer of the
 * mathematical equation is exactly 100.
 * Some seven-digit numbers are solvable, like 3141592 which can be solved as (3*1+4)*(1*5+9)+2 or
 * a single digit change of it 3146592, which can also be solved as 3*((-1)+(-4)/6+5*(9-2)), while
 * other seven-digit numbers simply cannot be solved, such as 0314157.
 * There are 63 ways to change a single digit in a seven-digit number.
 * Here's the challenge: Find a seven-digit number that is not solvable, but is made solvable by 62
 * of the 63 single-digit changes.
 * Bonus '*' if the number is prime.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int DEBUG = 0;

// rational number
struct r_t {
    int n, d;
    r_t(int n_ = 0, int d_ = 1) : n(n_), d(d_) {
        if(n == 0) { d = 1; }
        else if(d < 0) { n = -n; d = -d; }
    }
    r_t operator + (const r_t& r) const { return { n * r.d + r.n * d, d * r.d }; }
    r_t operator - (const r_t& r) const { return { n * r.d - r.n * d, d * r.d }; }
    r_t operator * (const r_t& r) const { return { n * r.n, d * r.d }; }
    r_t operator / (const r_t& r) const { return { n * r.d, d * r.n }; }
    bool operator == (int r) const {
        if(n == 0 && r == 0) return true;
        return n == r * d;
    }
};

void print(const r_t& r) {
    auto r_ = r;
    for(int i = r.d; i > 1; i--) {
        if(r_.n % i == 0 && r_.d % i == 0) { r_.n /= i; r_.d /= i; }
    }
    if(r_.d == 1) printf("%d", r_.n);
    else printf("%d/%d", r_.n, r_.d);
}

int solve(r_t* d, int n) {
    if(n == 1) {
        if(d[0] == 100 || d[0] == -100) return 1;
        return 0;
    }
    auto d_ = d + n;
    for(int pos = 0; pos < n - 1; pos++) {
        if(d[pos] == 0 && d[pos + 1] == 0) continue;

        memcpy(d_, d, pos * sizeof(d[0]));
        memcpy(d_ + pos + 1, d + pos + 2, (n - pos - 1) * sizeof(d[0]));

        d_[pos] = d[pos] + d[pos + 1];
        if(solve(d_, n - 1)) {
            if(DEBUG) { print(d[pos]); printf(" + "); print(d[pos + 1]); printf("\n"); }
            return 1;
        }
        d_[pos] = d[pos] - d[pos + 1];
        if(solve(d_, n - 1)) {
            if(DEBUG) { print(d[pos]); printf(" - "); print(d[pos + 1]); printf("\n"); }
            return 1;
        }
        d_[pos] = d[pos] * d[pos + 1];
        if(solve(d_, n - 1)) {
            if(DEBUG) { print(d[pos]); printf(" * "); print(d[pos + 1]); printf("\n"); }
            return 1;
        }
        if(d[pos + 1] == 0) continue;
        d_[pos] = d[pos] / d[pos + 1];
        if(solve(d_, n - 1)) {
            if(DEBUG) { print(d[pos]); printf(" / "); print(d[pos + 1]); printf("\n"); }
            return 1;
        }
    }

    return 0;
}

const int N = 7;
const int n_max = pow(10, N);

r_t d[256];

int solve(int n) {
    for(int i = N - 1; i >= 0; i--) { d[i] = n % 10; n /= 10; }

    if(solve(d, N)) {
        for(int i = 0; i < N; i++) printf("%d", d[i].n);
        printf(" => 100\n");
        return 1;
    }

    printf("\n");
    for(int i = 0; i < N; i++) printf("%d", d[i].n);
    printf("\n");

    int k = 0;
    auto d_ = d + N;
    memcpy(d_, d, N * sizeof(d[0]));
    for(int i = 0; i < N; i++) for(int j = 1; j < 10; j++) {
        d_[i].n = (d[i].n + j) % 10;
        printf("  => "); for(int i = 0; i < N; i++) printf("%d", d_[i].n);
        if(solve(d_, N)) { k += 1; printf(" => 100"); }
        printf("\n");
        d_[i].n = d[i].n;
    }
    if(k == 62) exit(0);

    return 0;
}

int main() {
    solve(397799);
//    solve(765533);

    int primes[1024 * 1024] = { 1, 2, 3 };
    int n_p = 3;

    for(int p = 5; p < n_max; p += 2) {
        int ok = 1;
        for(int i = 2, r_max = sqrtf(p); i < n_p; i++) {
            int r = primes[i];
            if(r > r_max) break;
            if(p % r == 0) { ok = 0; break; }
        }
        if(ok) {
//            printf("primes[%d] = %d\n", n_p, p);
            primes[n_p++] = p;
            if(n_p >= sizeof(primes) / sizeof(primes[0])) exit(1);
        }
    }
    printf("n_p = %d\n", n_p);

    for(int i = 0; i < n_p; i++) {
        solve(primes[i]);
    }

    return 0;
}
