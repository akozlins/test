/**
 * http://www.research.ibm.com/haifa/ponderthis/challenges/May2015.html
 *
 * Three people are playing the following betting game.
 * Every five minutes, a turn takes place in which a random player rests and the other two bet
 * against one another with all of their money.
 * The player with the smaller amount of money always wins,
 * doubling his money by taking it from the loser.
 * For example, if the initial amounts of money are 1, 4, and 6,
 * then the result of the first turn can be either
 * 2,3,6 (1 wins against 4);
 * 1,8,2 (4 wins against 6); or
 * 2,4,5 (1 wins against 6).
 * If two players with the same amount of money play against one another,
 * the game immediately ends for all three players.
 * Find initial amounts of money for the three players, where none of the three has more than 255,
 * and in such a way that the game cannot end in less than one hour. (So at least 12 turns)
 * In the example above (1,4,6), there is no way to end the game in less than 15 minutes.
 * All numbers must be positive integers.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>

const int s_max = 253 + 254 + 255;

const uint8_t t_max = 16;

void sort(unsigned& a, unsigned& b, unsigned& c) {
    if(a > b) std::swap(a, b);
    if(b > c) std::swap(b, c);
    if(a > b) std::swap(a, b);
}

uint8_t ts[s_max][s_max];

void play(unsigned a, unsigned b, unsigned c, uint8_t t) {
    if(t > 1 && (a == b || a == c || b == c)) return;

    sort(a, b, c);
    if(ts[a][b] <= t) return;
    ts[a][b] = t;

    if(t >= t_max) return;

    if(a % 2 == 0) {
        auto a_ = a / 2, b_ = b + a_, c_ = c + a_;
        if(a_ < b_) play(a_, b_, c, t + 1);
        if(a_ < c_) play(a_, c_, b, t + 1);
    }
    if(b % 2 == 0) {
        auto b_ = b / 2, a_ = a + b_, c_ = c + b_;
        if(b_ < a_) play(b_, a_, c, t + 1);
        if(b_ < c_) play(b_, c_, a, t + 1);
    }
    if(c % 2 == 0) {
        auto c_ = c / 2, a_ = a + c_, b_ = b + c_;
        if(c_ < a_) play(c_, a_, b, t + 1);
        if(c_ < b_) play(c_, b_, a, t + 1);
    }
}

int main() {
    for(int s = 3; s <= s_max; s++) {
        memset(ts, 0xFF, sizeof(ts));
        for(int i = 1; i < s; i++) { int j = i;
            if(i + j >= s) break;
            int k = s - i - j;
            play(i, j, k, 1);
        }
        for(int i = 1; i <= 255; i++) for(int j = 1; j <= 255; j++) {
            if(i + j >= s) break;
            int k = s - i - j;
            if(!(k <= 255)) continue;
            auto t = ts[i][j];
            if(11 <= t && t < 0xFF) printf("%d = %d + %d + %d => %d\n", s, i, j, k, t);
        }
    }

    return 0;
}
