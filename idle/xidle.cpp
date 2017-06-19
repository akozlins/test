
#include "xidle.h"

#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <X11/extensions/scrnsaver.h>

Display* display = nullptr;

x_idle_t::x_idle_t() {
    display = XOpenDisplay(":0");
    if(!display) return;

    int event_basep, error_basep;
    if(!XScreenSaverQueryExtension(display, &event_basep, &error_basep)) {
        close();
        return;
    }
}

x_idle_t::~x_idle_t() {
    close();
}

/*!
 * This function works around an XServer idleTime bug in the
 * XScreenSaverExtension if dpms is running. In this case the current
 * dpms-state time is always subtracted from the current idletime.
 * This means: XScreenSaverInfo->idle is not the time since the last
 * user activity, as descriped in the header file of the extension.
 * This result in SUSE bug # and sf.net bug #. The bug in the XServer itself
 * is reported at https://bugs.freedesktop.org/buglist.cgi?quicksearch=6439.
 *
 * Workaround: Check if if XServer is in a dpms state, check the
 *             current timeout for this state and add this value to
 *             the current idle time and return.
 */
unsigned long idle_dpms(Display* display, unsigned long idleTime) {
    int dummy;
    if (DPMSQueryExtension(display, &dummy, &dummy) == 0) goto exit;
    if (DPMSCapable(display) == 0) goto exit;

    CARD16 standby, suspend, off;
    DPMSGetTimeouts(display, &standby, &suspend, &off);

    CARD16 level;
    BOOL state;
    DPMSInfo(display, &level, &state);
    if(!state) goto exit;

    switch (level) {
    case DPMSModeStandby:
        /* this check is a littlebit paranoid, but be sure */
        if (idleTime < (unsigned) (standby * 1000)) {
            idleTime += (standby * 1000);
        }
        break;
    case DPMSModeSuspend:
        if (idleTime < (unsigned) ((suspend + standby) * 1000)) {
            idleTime += ((suspend + standby) * 1000);
        }
        break;
    case DPMSModeOff:
        if (idleTime < (unsigned) ((off + suspend + standby) * 1000)) {
            idleTime += ((off + suspend + standby) * 1000);
        }
        break;
    case DPMSModeOn:
    default:
        break;
    }

    exit:
    return idleTime;
}

unsigned long x_idle_t::idle() {
    if(!display) return ULONG_MAX;

    XScreenSaverInfo ssi;
    if (!XScreenSaverQueryInfo(display, DefaultRootWindow(display), &ssi)) {
        return 0;
    }
    return ssi.idle;
}

void x_idle_t::close() {
    if(display) XCloseDisplay(display);
    display = nullptr;
}
