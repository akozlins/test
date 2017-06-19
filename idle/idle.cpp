
#include "idle.h"

#include <list>
#include <thread>

#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

#include <linux/input.h>

#include <sys/stat.h>



static uint64_t clock_gettime() {
    timespec t;
    ::clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}



i_idle_t::i_idle_t() : last(0), stop(0) {
    for(int i = 0;; i++) {
        char path[256];
        sprintf(path, "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY);
        if(fd == -1) break;

        printf("%.256s\n", path);

        char name[256];
        if(ioctl(fd, EVIOCGNAME(sizeof(name)), name) > 0) {
            printf("  name: %.256s\n", name);
        }
        unsigned version;
        ioctl(fd, EVIOCGVERSION, &version);
        printf("  version: %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);
        input_id id;
        ioctl(fd, EVIOCGID, &id);
        printf("  id: Bus=%04x Vendor=%04x Product=%04x Version=%04x\n", id.bustype, id.vendor, id.product, id.version);

        unsigned long evbit = 0;
        ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
        int ev_key = evbit & (1 << EV_KEY);

        unsigned char keybit[KEY_MAX / 8 + 1];
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), &keybit);

        printf("  ");
        if(ev_key) printf("EV_KEY");
        if(keybit['a'/8] & (1 << ('a' % 8))) printf(" + 'a'");
        printf("\n");

        if(ev_key) fds.push_back(fd);
        else ::close(fd);
    }

    fds.sort();

    thr = std::thread(&i_idle_t::run, this);
}

i_idle_t::~i_idle_t() {
    close();
}

void i_idle_t::run() {
    if(fds.empty()) return;
    int nfds = fds.back();

    fd_set fds_;
    while(!stop) {
        FD_ZERO(&fds_);
        for(int fd : fds) FD_SET(fd, &fds_);
        if(select(nfds + 1, &fds_, nullptr, nullptr, nullptr) == -1) return;

        for(int fd : fds) if(FD_ISSET(fd, &fds_)) {
            struct input_event event;
            if(read(fd, &event, sizeof(event)) != sizeof(event)) return;
            last = clock_gettime();

/*            printf("%ld.%06ld: "
                   "type=%02x "
                   "code=%02x "
                   "value=%02x\n",
                   event.time.tv_sec,
                   event.time.tv_usec,
                   event.type,
                   event.code,
                   event.value
            );*/
        }
    }
}

/*int run2 () {
    event_base* eb = event_base_new();
    for(int fd : fds) {
        event* e = event_new(eb, fd, EV_READ | EV_PERSIST, [] (int fd, short, void*) {
            struct input_event event;
            read(fd, &event, sizeof(event));
            last = time(NULL);
        }, (void*)&last);
        event_add(e, nullptr);
    }
    event_base_dispatch(eb);
    return 0;
}*/

unsigned long i_idle_t::idle() {
    if(last == 0 || !thr.joinable()) return ULONG_MAX;
    return clock_gettime() - last;
}

void i_idle_t::close() {
    stop = 1;
    for(int fd : fds) ::close(fd);
    if(thr.joinable()) thr.join();
    fds.clear();
}



w_idle_t::w_idle_t() {
}

w_idle_t::~w_idle_t() {
}

unsigned long w_idle_t::idle() {
    uint64_t t = 0;

    utmpname(UTMP_FILE);
    setutent();
    while(true) {
        struct utmp *u = getutent();
        if (!u) break;
        if (u->ut_type != USER_PROCESS) continue;
        if (u->ut_user[0] == '\0') continue;

        char tty[5 + UT_LINESIZE + 1] = "/dev/";
        for (int i = 0; i < UT_LINESIZE; i++) {
            /* clean up tty if garbled */
            if (isalnum(u->ut_line[i]) || (u->ut_line[i] == '/'))
                tty[i + 5] = u->ut_line[i];
            else
                tty[i + 5] = '\0';
        }
        tty[5 + UT_LINESIZE] = '\0';

        char uname[UT_NAMESIZE + 1] = "";
        strncpy(uname, u->ut_user, UT_NAMESIZE);
        uname[UT_NAMESIZE] = '\0';

        struct stat sbuf;
        if (stat(tty, &sbuf) != 0) continue;
//        printf("%s : %s => %lu\n", uname, tty, time(NULL) - t);
        if(sbuf.st_atime > t) t = sbuf.st_atime;
    }
    endutent();

    if(t == 0) return ULONG_MAX;

    return clock_gettime() - 1000 * t;
}
