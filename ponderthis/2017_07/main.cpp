
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <map>
#include <string>

#include <mpfr.h>

const int N = 4000000;

mpfr_t sins[91];
char c863172[91];

char buffer[1024*1024] {};

int main() {
    mpfr_t pi;
    mpfr_init2(pi, N);
    mpfr_const_pi(pi, MPFR_RNDN);

/*    mpfr_t x, y;
    mpfr_init2(x, N);
    mpfr_init2(y, N);
    mpfr_set_d(x, 18, MPFR_RNDN);
    mpfr_set_d(y, 54, MPFR_RNDN);
    mpfr_div_d(x, x, 180, MPFR_RNDN);
    mpfr_div_d(y, y, 180, MPFR_RNDN);
    mpfr_mul(x, x, pi, MPFR_RNDN);
    mpfr_mul(y, y, pi, MPFR_RNDN);
    mpfr_sin(x, x, MPFR_RNDN);
    mpfr_sin(y, y, MPFR_RNDN);
    mpfr_mul(x, x, y, MPFR_RNDN);
    mpfr_sprintf(buffer, "%.1001000RNf", x);
    printf("%.100s\n", buffer);
    return 0;*/

    FILE* fin = nullptr; //fin = fopen("fin.mpfr", "r");
    FILE* fout = nullptr; //fout = fopen("fout.mpfr", "w");

    for(int i = 0; i <= 90; i++) {
        mpfr_init2(sins[i], N);
        if(!fin) {
            mpfr_set_d(sins[i], i, MPFR_RNDN);
            mpfr_div_d(sins[i], sins[i], 180, MPFR_RNDN);
            mpfr_mul(sins[i], sins[i], pi, MPFR_RNDN);
            mpfr_sin(sins[i], sins[i], MPFR_RNDN);
        }
        else {
            mpfr_inp_str(sins[i], fin, 62, MPFR_RNDN);
        }
        if(fout) {
            mpfr_out_str(fout, 62, 0, sins[i], MPFR_RNDN);
            fwrite("\n", 1, 1, fout);
        }

        mpfr_sprintf(buffer, "%.1001000RNf", sins[i]);
        c863172[i] = buffer[863172 + 1];

        printf("%i: %.40s => c863172 = %c\n", i, buffer, c863172[i]);
    }
    if(fin) fclose(fin);
    if(fout) fclose(fout);

    mpfr_t x;
    mpfr_init2(x, N);

    std::map<int, int> m;

    std::string seq = "0123456789";
    do {
        int i4 = atoi(seq.substr(0,4).c_str());
        int i6 = atoi(seq.substr(4,6).c_str());

        int i4_ = i4 % 80;
        if(i4_ > 90) i4_ = 180 - i4_;
        if(c863172[i4_] == '0') continue;

        int i6_ = i6 % 180;
        if(i6_ > 90) i6_ = 180 - i6_;
        if(c863172[i6_] == '0') continue;
        
        int n = m[i4_ + i6_ * 100];
        if(n == 0) {
            mpfr_mul(x, sins[i4_], sins[i6_], MPFR_RNDN);
            mpfr_sprintf(buffer, "%.1001000RNf", x);
            n = 0; for(int i = 2; i < 1000002; i++) if(buffer[i] == '0') n++;
            m[i4_ + i6_ * 100] = m[i4_ * 100 + i6_] = n;
        }
        if(n >= 200000) printf("!!!!!!!!\n");
        printf("sin(%d) * sin(%d) = sin(%d) * sin(%d) => n('0') = %d\n", i4, i6, i4_, i6_, n);

    } while(std::next_permutation(seq.begin(), seq.end()));

    return 0;
}
