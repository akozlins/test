#ifndef __IDLE_H
#define __IDLE_H

#include <list>
#include <thread>

#include <stdint.h>

inline
void print_idle(unsigned long idle) {
    if(idle >= 99 * 60 * 60 * 1000) printf("--:--:--.-");
    else printf("%02lu:%02lu:%02lu.%01lu", (idle / 1000 / 60 / 60), (idle / 1000 / 60) % 60, (idle / 1000) % 60, (idle % 1000) / 100);
}

struct i_idle_t {
    std::list<int> fds;

    i_idle_t();
    ~i_idle_t();

    std::thread thr;
    void run();

    volatile uint64_t last;
    unsigned long idle();

    int cfd[2];
    void close();
};

struct w_idle_t {
    w_idle_t();
    ~w_idle_t();

    volatile uint64_t last;
    unsigned long idle();

    void close();
};

#endif // __IDLE_H
