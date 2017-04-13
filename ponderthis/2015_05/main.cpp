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

const uint8_t t_max = 16;

void sort(int& a, int& b, int& c) {
    if(a > b) std::swap(a, b);
    if(b > c) std::swap(b, c);
    if(a > b) std::swap(a, b);
}

uint8_t ts[1024][1024];

void play(int a, int b, int c, uint8_t t = 1) {
    if(t > 1 && (a == b || b == c || c == a)) return;

    sort(a, b, c);
    if(0 < ts[a][b] && ts[a][b] <= t) return;
    ts[a][b] = t;

    if(t >= t_max) return;

    // ab
    if(a % 2 == 0 && a / 2 < b + a / 2) play(a / 2    , b + a / 2, c        , t + 1);
    if(b % 2 == 0 && b / 2 < a + b / 2) play(a + b / 2, b / 2    , c        , t + 1);
    // bc
    if(b % 2 == 0 && b / 2 < c + b / 2) play(a        , b / 2    , c + b / 2, t + 1);
    if(c % 2 == 0 && c / 2 < b + c / 2) play(a        , b + c / 2, c / 2    , t + 1);
    // ac
    if(a % 2 == 0 && a / 2 < c + a / 2) play(a / 2    , b        , c + a / 2, t + 1);
    if(c % 2 == 0 && c / 2 < a + c / 2) play(a + c / 2, b        , c / 2    , t + 1);
}

int main() {
    for(int s = 1 + 2 + 3; s <= 253 + 254 + 255; s++) {
        memset(ts, 0, 1024 * 1024);
        for(int i = 1; i < s; i++) {
            int j = i;
            int k = s - i - j;
            if(k < 0) break;
            play(i, j, k);
        }
        for(int i = 1; i <= 255; i++) for(int j = 1; j <= 255; j++) {
            int k = s - i - j;
            if(!(0 < k && k <= 255)) continue;
            int t = ts[i][j];
            if(t < 11) continue;
            printf("%d = %d + %d + %d => %d\n", s, i, j, k, t);
        }
    }

    return 0;
}
