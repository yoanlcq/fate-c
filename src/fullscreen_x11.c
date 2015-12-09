#include <stdbool.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xcbext.h>
#include <xcb/xproto.h>
#include <xcb/xinput.h>

/* Most of the code come from SFML's source. */
static xcb_atom_t getAtom(xcb_connection_t *connection, const char *name, 
                          bool only_if_exists) {
    xcb_generic_error_t *err = NULL;
    xcb_intern_atom_cookie_t cookie;
    xcb_intern_atom_reply_t *reply;
    xcb_atom_t atom;

    cookie = xcb_intern_atom(connection, only_if_exists, strlen(name), name);
    reply = xcb_intern_atom_reply(connection, cookie, &err);
    atom = reply->atom;
    if(error || !reply) {
        fate_logf_video("Could not get atom \"%s\" (error %d).\n",
                        name, error->error_code);
        if(error)
            free(error);
        return XCB_ATOM_NONE;
    }
    free(reply);
    return atom;
}

static xcb_screen_t *screen_of_display(xcb_connection_t *c, int screen) {
    xcb_screen_iterator_t iter;
    iter = xcb_setup_roots_iterator(xcb_get_setup(c));
    for(; iter.rem; --screen, xcb_screen_next(&iter))
        if(screen == 0)
            return iter.data;
    return NULL;
}
static xcb_window_t default_root_window(void) {
    xcb_connection_t *c;
    xcb_screen_t     *screen;
    int               screen_default_nbr;

    c = xcb_connect(NULL, &screen_default_nbr);
    screen = screen_of_display(c, screen_default_nbr);
    xcb_disconnect(c);
    return screen ? screen->root : NULL;
}

static bool ewmhSupported(xcb_connection_t *connection) {
    static bool checked = false;
    static bool ewmhSupported = false;

    if(checked)
        return ewmhSupported;

    checked = true;

    xcb_atom_t netSupportingWmCheck = getAtom("_NET_SUPPORTING_WM_CHECK", true);
    xcb_atom_t netSupported = getAtom("_NET_SUPPORTED", true);

    if(!netSupportingWmCheck || !netSupported)
        return false;

    xcb_generic_error_t error = NULL;
    xcb_get_property_cookie_t pcookie;
    xcb_get_property_reply_t *rootSupportingWindow;

    pcookie = xcb_get_property(connection, 0,
                  default_root_window(),
                  netSupportingWmCheck,
                  XCB_ATOM_WINDOW,
                  0,
                  1
                );
    rootSupportingWindow = xcb_get_property_reply(connection, pcookie, &error);
    if(error)
        free(error);

    if(!rootSupportingWindow)
        return false;
    else if(rootSupportingWindow->length != 1) {
        free(rootSupportingWindow);
        return false;
    }

    xcb_window_t* rootWindow = (xcb_window_t*) xcb_get_property_value(rootSupportingWindow);
    free(rootSupportingWindow);

    if(!rootWindow)
        return false;

    pcookie = xcb_get_property(connection, 0,
                  *rootWindow,
                  netSupportingWmCheck,
                  XCB_ATOM_WINDOW,
                  0,
                  1
                );
    childSupportingWindow = xcb_get_property_reply(connection, pcookie, &error);
    if(error)
        free(error);

    if(!childSupportingWindow)
        return false;
    else if(childSupportingWindow->length != 1) {
        free(childSupportingWindow);
        return false;
    }

    xcb_window_t* childWindow = (xcb_window_t*)xcb_get_property_value(childSupportingWindow);

    if(!childWindow)
        return false;

    // Conforming window managers should return the same window for both queries
    if(*rootWindow != *childWindow)
        return false;

    ewmhSupported = true;

    return true;
}

static void grabFocus(void)
{
    xcb_atom_t netActiveWindow = XCB_ATOM_NONE;

    if(ewmhSupported())
        netActiveWindow = getAtom("_NET_ACTIVE_WINDOW");

    if(netActiveWindow)
    {
        xcb_client_message_event_t event;
        std::memset(&event, 0, sizeof(event));

        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = m_window;
        event.format = 32;
        event.sequence = 0;
        event.type = netActiveWindow;
        event.data.data32[0] = 1; // Normal application
        event.data.data32[1] = XCB_CURRENT_TIME;
        event.data.data32[2] = 0; // We don't know the currently active window

        ScopedXcbPtr<xcb_generic_error_t> activeWindowError(xcb_request_check(
            m_connection,
            xcb_send_event_checked(
                m_connection,
                0,
                XCBDefaultRootWindow(m_connection),
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                reinterpret_cast<char*>(&event)
            )
        ));

        if(activeWindowError)
            err() << "Setting fullscreen failed, could not send \"_NET_ACTIVE_WINDOW\" event" << std::endl;
    }
    else
    {
        ScopedXcbPtr<xcb_generic_error_t> setInputFocusError(xcb_request_check(
            m_connection,
            xcb_set_input_focus(
                m_connection,
                XCB_INPUT_FOCUS_POINTER_ROOT,
                m_window,
                XCB_CURRENT_TIME
            )
        ));

        if(setInputFocusError)
        {
            err() << "Failed to change active window (set_input_focus)" << std::endl;
            return;
        }

        const uint32_t values[] = {XCB_STACK_MODE_ABOVE};

        ScopedXcbPtr<xcb_generic_error_t> configureWindowError(xcb_request_check(
            m_connection,
            xcb_configure_window(
                m_connection,
                m_window,
                XCB_CONFIG_WINDOW_STACK_MODE,
                values
            )
        ));

        if(configureWindowError)
            err() << "Failed to change active window (configure_window)" << std::endl;
    }
}



static void fullscreen_bypass_compositor(sfWindowHandle window) {

    xcb_connection_t* connection = xcb_connect(NULL, NULL);

    grabFocus();

    if(ewmhSupported())
    {
        xcb_atom_t netWmBypassCompositor = getAtom("_NET_WM_BYPASS_COMPOSITOR");

        if(netWmBypassCompositor)
        {
            static const Uint32 bypassCompositor = 1;

            // Not being able to bypass the compositor is not a fatal error
            if(!changeWindowProperty(netWmBypassCompositor, XCB_ATOM_CARDINAL, 32, 1, &bypassCompositor))
                err() << "xcb_change_property failed, unable to set _NET_WM_BYPASS_COMPOSITOR" << std::endl;
        }

        xcb_atom_t netWmState = getAtom("_NET_WM_STATE", true);
        xcb_atom_t netWmStateFullscreen = getAtom("_NET_WM_STATE_FULLSCREEN", true);

        if(!netWmState || !netWmStateFullscreen)
        {
            err() << "Setting fullscreen failed. Could not get required atoms" << std::endl;
            return;
        }

        xcb_client_message_event_t event;
        std::memset(&event, 0, sizeof(event));

        event.response_type = XCB_CLIENT_MESSAGE;
        event.window = m_window;
        event.format = 32;
        event.sequence = 0;
        event.type = netWmState;
        event.data.data32[0] = 1; // _NET_WM_STATE_ADD
        event.data.data32[1] = netWmStateFullscreen;
        event.data.data32[2] = 0; // No second property
        event.data.data32[3] = 1; // Normal window

        ScopedXcbPtr<xcb_generic_error_t> wmStateError(xcb_request_check(
            m_connection,
            xcb_send_event_checked(
                m_connection,
                0,
                XCBDefaultRootWindow(m_connection),
                XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                reinterpret_cast<char*>(&event)
            )
        ));

        if(wmStateError)
            err() << "Setting fullscreen failed. Could not send \"_NET_WM_STATE\" event" << std::endl;
    }
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

static void fullscreen_motif_wm_hints(Window win) {
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
        return;
    XMoveResizeWindow(dpy,win,0,0,vms[0].width,vms[0].height);
    XMapRaised(dpy,win);
    XGrabPointer(dpy,win,True,0,GrabModeAsync,GrabModeAsync,win,0L,CurrentTime);
    XGrabKeyboard(dpy,win,False,GrabModeAsync,GrabModeAsync,CurrentTime);

    XCloseDisplay(dpy);
}


bool fate_sfWindow_get_fullscreen(sfWindowHandle window) {
    
}
void fate_sfWindow_enable_fullscreen(sfWindowHandle window) {
    if(!fullscreen_bypass_compositor(window))
        fullscreen_motif_wm_hints(window);
}
void fate_sfWindow_disable_fullscreen(sfWindowHandle window) {
    /* TODO */
}
