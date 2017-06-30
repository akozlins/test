
#include "idle.h"

#include <list>
#include <thread>

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

#include <linux/input.h>

#include <sys/inotify.h>
#include <sys/stat.h>



static const std::string dev_input = "/dev/input/";



static uint64_t clock_gettime() {
    timespec t;
    ::clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}



i_idle_t::i_idle_t() : last(0) {
    if(pipe(cfd) != 0) {
        printf("error: failed to create pipe\n");
        cfd[0] = cfd[1] = -1;
    }
    if(cfd[0] == -1 || cfd[1] == -1) { last = ULONG_MAX; return; }
    fds.push_back(cfd[0]);

    do {
        nfd = inotify_init();
        if(nfd == -1) {
            printf("error: failed to init inotify");
            break;
        }

        // IN_ALL_EVENTS
        int wd = inotify_add_watch(nfd, dev_input.c_str(), IN_ACCESS | IN_MODIFY | IN_CREATE | IN_DELETE);
        if(wd == -1) {
            printf("error: inotify_add_watch failed\n");
            ::close(nfd);
            nfd = -1;
        }
        wd = inotify_add_watch(nfd, UTMP_FILE, IN_MODIFY);
        if(wd == -1) {
            printf("error: inotify_add_watch('%s') failed\n", UTMP_FILE);
        }
    } while(0);
    if(nfd == -1) { last = ULONG_MAX; return; }
    fds.push_back(nfd);

    DIR* dir = opendir(dev_input.c_str());
    if(dir) while(struct dirent* dent = readdir(dir)) {
        if(strncmp(dent->d_name, "event", 5) != 0) continue;

        std::string path = dev_input + dent->d_name;
        int fd = open(path.c_str(), O_RDONLY);
        if(fd == -1) {
            printf("warn: failed to open '%s'\n", path.c_str());
            continue;
        }

        unsigned long evbit = 0;
        ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
        int ev_key = evbit & (1 << EV_KEY);
        if(!ev_key) {
            ::close(fd);
            continue;
        }

        printf("%s\n", path.c_str());

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

        unsigned char keybit[KEY_MAX / 8 + 1];
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), &keybit);

        printf("  ");
        printf("EV_KEY");
        if(keybit['a'/8] & (1 << ('a' % 8))) printf(" + 'a'");
        printf("\n");

        fds.push_back(fd);
    }
    if(dir) closedir(dir);

    fds.sort();

    thr = std::thread(&i_idle_t::run, this);
}

i_idle_t::~i_idle_t() {
    close();
}

void i_idle_t::run() {
    // sizeof(struct inotify_event) + NAME_MAX + 1
    char buffer[1024];

    fd_set fds_;
    while(true) {
        fds.remove(-1);
        if(fds.empty()) {
            printf("error: no fds\n");
            last = ULONG_MAX; return;
        }

        FD_ZERO(&fds_);
        for(int fd : fds) FD_SET(fd, &fds_);

        int nfds = fds.back();
        if(select(nfds + 1, &fds_, nullptr, nullptr, nullptr) == -1) {
            printf("error: failed select\n");
            last = ULONG_MAX; return;
        }

        for(int& fd : fds) if(FD_ISSET(fd, &fds_)) {
            if(fd == cfd[0]) {
                last = ULONG_MAX; return;
            }

            int n = read(fd, buffer, sizeof(buffer));
            if(n == 0 || n == -1) {
                printf("warn: remove fd '%d'\n", fd);
                ::close(fd); fd = -1;
                last = ULONG_MAX; continue;
            }

            if(fd == nfd) {
                struct inotify_event* event = (struct inotify_event*)buffer;

#ifdef DEBUG
                if(event->len > 0) printf(" '%s'", event->name);
                printf(" (%d) -> ", event->wd);
                if(event->mask & IN_ACCESS) printf("IN_ACCESS");
                if(event->mask & IN_MODIFY) printf("IN_MODIFY");
                if(event->mask & IN_CREATE) printf("IN_CREATE");
                if(event->mask & IN_DELETE) printf("IN_DELETE");
                printf("\n");
#endif

                if(event->len == 0 && (event->mask & IN_ACCESS)) continue;
            }
            else {
                struct input_event* event = (struct input_event*)buffer;

#ifdef DEBUG
                printf("%ld.%06ld: "
                       "type=%02x "
                       "code=%02x "
                       "value=%02x\n",
                       event->time.tv_sec,
                       event->time.tv_usec,
                       event->type,
                       event->code,
                       event->value
                );
#endif
            }

            last = clock_gettime();
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
    if(last == ULONG_MAX || !thr.joinable()) return 0;
    if(last == 0) return ULONG_MAX;
    return clock_gettime() - last;
}

void i_idle_t::close() {
    if(cfd[0] == -1 || cfd[1] == -1) return;
    ::close(cfd[1]);
    if(thr.joinable()) thr.join();
    for(int fd : fds) ::close(fd);
    fds.clear();
    cfd[0] = cfd[1] = nfd = -1;
}



w_idle_t::w_idle_t() : last(0) {
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

#ifdef DEBUG
        if(clock_gettime() - 1000 * sbuf.st_atime < 1000) {
            printf(" %s\n", tty);
        }
#endif
    }
    endutent();

    if(1000 * t > last) last = 1000 * t;

    if(last == 0) return 0;
    return clock_gettime() - last;
}
