
#include "rand.h"

rand_t r;

int main() {
    for(int i = 0; i < 256; i++) {
        printf("%f\n", r.uniform(1, 2.5));
    }

    return 0;
}
