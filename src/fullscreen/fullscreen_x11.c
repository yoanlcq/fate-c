#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcbext.h>
#include <xcb/xproto.h>
#include <xcb/xinput.h>
#include <SFML/Window.h>
#include <fate/log.h>

#define FATE_XCB_INIT() \
    xcb_generic_error_t *xcberror
#define FATE_XCB_DECL(VAR, REQ) \
    xcb_##REQ##_cookie_t cookie_##VAR; \
    xcb_##REQ##_reply_t *VAR
#define FATE_XCB_SEND(VAR, REQ, CONN, ...) \
    cookie_##VAR = xcb_##REQ(CONN, __VA_ARGS__)
#define FATE_XCB_RECV(VAR, REQ, CONN, ONERROR) \
    VAR = xcb_##REQ##_reply(CONN, cookie_##VAR, &xcberror); \
    if(!VAR) { \
        fate_logf_video("xcb_%s failed with error code %d.\n", #REQ, xcberror->error_code); \
        free(xcberror); \
        ONERROR; \
    }
#define FATE_XCB_DISCARD(CONN, VAR) xcb_discard_reply(CONN, cookie_##VAR.sequence)
#define FATE_XCB_FREE(VAR) free(VAR)

/* Most of the code comes from SFML's source. */

static xcb_screen_t *screen_of_display(xcb_connection_t *c, int screen) {
    xcb_screen_iterator_t iter;
    iter = xcb_setup_roots_iterator(xcb_get_setup(c));
    for(; iter.rem; --screen, xcb_screen_next(&iter))
        if(screen == 0)
            return iter.data;
    return NULL;
}
static xcb_window_t default_root_window(xcb_connection_t *c, int default_screen_nbr) {
    xcb_screen_t *screen = screen_of_display(c, default_screen_nbr);
    return screen ? screen->root : 0;
}

static inline void could_not_get_atom(const char *name) {
    fate_logf_video("Could not get atom \"%s\".\n", name);
}

static bool ewmhSupported(xcb_connection_t *conn, int default_screen_nbr) {
    static bool checked = false;
    static bool ewmhSupported = false;

    if(checked)
        return ewmhSupported;

    checked = true;

    FATE_XCB_INIT();

    FATE_XCB_DECL(ns[2], intern_atom);
    FATE_XCB_SEND(ns[0], intern_atom, conn, true, strlen("_NET_SUPPORTING_WM_CHECK"), "_NET_SUPPORTING_WM_CHECK");
    FATE_XCB_SEND(ns[1], intern_atom, conn, true, strlen("_NET_SUPPORTED"), "_NET_SUPPORTED");
    FATE_XCB_RECV(ns[0], intern_atom, conn, 
            FATE_XCB_DISCARD(conn, ns[1]);
            could_not_get_atom("_NET_SUPPORTING_WM_CHECK");
            return false);
    FATE_XCB_RECV(ns[1], intern_atom, conn, 
            could_not_get_atom("_NET_SUPPORTED");
            return false);
    xcb_atom_t netSupportingWmCheck = ns[0]->atom;
    xcb_atom_t netSupported = ns[1]->atom;
    FATE_XCB_FREE(ns[0]);
    FATE_XCB_FREE(ns[1]);
    if(!netSupportingWmCheck || !netSupported)
        return false;

    FATE_XCB_DECL(rootSupportingWindow, get_property);
    FATE_XCB_SEND(rootSupportingWindow, get_property, conn, 0, default_root_window(conn, default_screen_nbr), netSupportingWmCheck, XCB_ATOM_WINDOW, 0, 1);
    FATE_XCB_RECV(rootSupportingWindow, get_property, conn, return false);
    if(rootSupportingWindow->length != 1) {
        FATE_XCB_FREE(rootSupportingWindow);
        return false;
    }
    xcb_window_t* rootWindow = (xcb_window_t*) xcb_get_property_value(rootSupportingWindow);
    FATE_XCB_FREE(rootSupportingWindow);
    if(!rootWindow)
        return false;

    FATE_XCB_DECL(childSupportingWindow, get_property);
    FATE_XCB_SEND(childSupportingWindow, get_property, conn, 0, *rootWindow, netSupportingWmCheck, XCB_ATOM_WINDOW, 0, 1);
    FATE_XCB_RECV(childSupportingWindow, get_property, conn, return false);
    if(childSupportingWindow->length != 1) {
        FATE_XCB_FREE(childSupportingWindow);
        return false;
    }
    xcb_window_t* childWindow = (xcb_window_t*) xcb_get_property_value(childSupportingWindow);
    FATE_XCB_FREE(childSupportingWindow);
    if(!childWindow)
        return false;

    /* Conforming window managers should return the same window for both queries */
    if(*rootWindow != *childWindow)
        return false;

    ewmhSupported = true;

    return true;
}

static void grabFocus(xcb_connection_t *conn, xcb_window_t win, int default_screen_nbr)
{
    FATE_XCB_INIT();
    xcb_atom_t netActiveWindow = XCB_ATOM_NONE;

    if(ewmhSupported(conn, default_screen_nbr)) {
        FATE_XCB_DECL(naw, intern_atom);
        FATE_XCB_SEND(naw, intern_atom, conn, false, strlen("_NET_ACTIVE_WINDOW"), "_NET_ACTIVE_WINDOW");
        FATE_XCB_RECV(naw, intern_atom, conn, );
        netActiveWindow = naw->atom;
        FATE_XCB_FREE(naw);
    }

    if(netActiveWindow)
    {
        xcb_client_message_event_t event;
        memset(&event, 0, sizeof(event));

        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = win;
        event.format = 32;
        event.sequence = 0;
        event.type = netActiveWindow;
        event.data.data32[0] = 1; // Normal application
        event.data.data32[1] = XCB_CURRENT_TIME;
        event.data.data32[2] = 0; // We don't know the currently active window

        xcb_void_cookie_t cookie = xcb_send_event_checked (
                conn,
                0,
                default_root_window(conn, default_screen_nbr),
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                (char*)&event
            );

        xcb_generic_error_t *activeWindowError = xcb_request_check(conn, cookie);

        if(activeWindowError)
            fate_logf_video("Setting fullscreen failed: could not send \"_NET_ACTIVE_WINDOW\" event.\n");
    }
    else
    {
        xcb_void_cookie_t cookie = xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, win, XCB_CURRENT_TIME);
        xcb_generic_error_t *setInputFocusError = xcb_request_check(conn, cookie);

        if(setInputFocusError) {
            fate_logf_video("Could not change active window (xcb_set_input_focus).\n");
            return;
        }

        const uint32_t values[] = {XCB_STACK_MODE_ABOVE};
        cookie = xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_STACK_MODE, values);
        xcb_generic_error_t *configureWindowError = xcb_request_check(conn, cookie);

        if(configureWindowError)
            fate_logf_video("Failed to change active window (xcb_configure_window).\n");
    }
}

static inline void could_not_bypass_compositor(const char *s) {
    fate_logf_video("Could not set fullscreen by bypassing compositor: %s\n", s);
}

static bool fullscreen_bypass_compositor(sfWindowHandle win) {

    FATE_XCB_INIT();
    int default_screen_nbr;
    xcb_connection_t* conn = xcb_connect(NULL, &default_screen_nbr);

    grabFocus(conn, win, default_screen_nbr);

    if(!ewmhSupported(conn, default_screen_nbr)) {
        could_not_bypass_compositor("EWMH is not supported.\n");
        return false;
    }

    FATE_XCB_DECL(nw[3], intern_atom);
    FATE_XCB_SEND(nw[0], intern_atom, conn, false, strlen("_NET_WM_BYPASS_COMPOSITOR"), "_NET_WM_BYPASS_COMPOSITOR");
    FATE_XCB_SEND(nw[1], intern_atom, conn, true,  strlen("_NET_WM_STATE"), "_NET_WM_STATE");
    FATE_XCB_SEND(nw[2], intern_atom, conn, true,  strlen("_NET_WM_STATE_FULLSCREEN"), "_NET_WM_STATE_FULLSCREEN");
    FATE_XCB_RECV(nw[0], intern_atom, conn, );
    FATE_XCB_RECV(nw[1], intern_atom, conn, could_not_get_atom("_NET_WM_STATE"));
    FATE_XCB_RECV(nw[2], intern_atom, conn, could_not_get_atom("_NET_WM_STATE_FULLSCREEN"));

    xcb_atom_t netWmBypassCompositor = nw[0] ? nw[0]->atom : XCB_ATOM_NONE;
    xcb_atom_t netWmState            = nw[1] ? nw[1]->atom : XCB_ATOM_NONE;
    xcb_atom_t netWmStateFullscreen  = nw[2] ? nw[2]->atom : XCB_ATOM_NONE;

    if(nw[0]) FATE_XCB_FREE(nw[0]);
    if(nw[1]) FATE_XCB_FREE(nw[1]);
    if(nw[2]) FATE_XCB_FREE(nw[2]);

    if(!netWmState || !netWmStateFullscreen) {
        could_not_bypass_compositor("Could not get the required atoms.");
        return false;
    }

    if(netWmBypassCompositor) {
        static const uint32_t bypassCompositor = 1;
        /* Not being able to bypass the compositor is not a fatal error. */
        xcb_generic_error_t *error;
        xcb_void_cookie_t cookie;

        cookie = xcb_change_property_checked(conn, XCB_PROP_MODE_REPLACE, win, netWmBypassCompositor, XCB_ATOM_CARDINAL, 32, 1, &bypassCompositor);
        error = xcb_request_check(conn, cookie);
        if(error)
            fate_logf_video("Warning : could not set _NET_WM_BYPASS_COMPOSITOR.");
    } 

    xcb_client_message_event_t event;
    memset(&event, 0, sizeof(event));

    event.response_type = XCB_CLIENT_MESSAGE;
    event.window = win;
    event.format = 32;
    event.sequence = 0;
    event.type = netWmState;
    event.data.data32[0] = 1; // _NET_WM_STATE_ADD
    event.data.data32[1] = netWmStateFullscreen;
    event.data.data32[2] = 0; // No second property
    event.data.data32[3] = 1; // Normal window

    xcb_void_cookie_t cookie = xcb_send_event_checked (
            conn,
            0,
            default_root_window(conn, default_screen_nbr),
            XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
            (char*)&event
        );

    xcb_generic_error_t *wmStateError = xcb_request_check(conn, cookie);

    if(wmStateError) {
        could_not_bypass_compositor("Could not send \"_NET_WM_STATE\" event.");
        return false;
    }
    return true;
}

/* From http://tonyobryan.com/index.php?article=9 */
struct Hints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long          inputMode;
    unsigned long status;
};
typedef struct Hints Hints;

static bool fullscreen_motif_wm_hints(Window win) {
    /* We can't access SFML's connection. Duh. */
    Display *dpy = XOpenDisplay(NULL);
    Hints hints = {0,0,0,0,0};
    Atom property;

    hints.flags = 2;
    hints.decorations = 0;
    property = XInternAtom(dpy, "_MOTIF_WM_HINTS", True);
    XChangeProperty(dpy, win, property, property, 
            32, PropModeReplace, (const unsigned char*) &hints, 5);
    size_t cnt = 1;
    const sfVideoMode *vms = sfVideoMode_getFullscreenModes(&cnt);
    if(cnt <= 0)
        return false;
    XMoveResizeWindow(dpy,win,0,0,vms[0].width,vms[0].height);
    XMapRaised(dpy,win);
    XGrabPointer(dpy,win,True,0,GrabModeAsync,GrabModeAsync,win,0L,CurrentTime);
    XGrabKeyboard(dpy,win,False,GrabModeAsync,GrabModeAsync,CurrentTime);

    XCloseDisplay(dpy);
    return true;
}

void fate_sfWindow_enable_fullscreen(const sfWindow *window) {
    sfWindowHandle handle = sfWindow_getSystemHandle(window);
    if(!fullscreen_bypass_compositor(handle))
        fullscreen_motif_wm_hints(handle);
}
void fate_sfWindow_disable_fullscreen(sfWindow *window) {
    /* TODO */
}
