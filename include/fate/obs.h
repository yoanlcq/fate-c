struct fate_version {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
};
typedef struct fate_version fate_version;

/* OBS stands for Observer. */

struct fate_obs_header {
    fate_version version; /* <- Required to allow for multiple versions 
                           *    of a plugin to be present. */
    fate_version engine_version;
    const uint8_t *name; /* <- Acts as an identifier. */
    const uint8_t *description; /* <- Put some more info for the user. */
    void (*event_handler)(fate_obs_event*);
};
typedef struct fate_obs_header fate_obs_header;

/* These are for creating internal observers. Plugins need not (nor should
 * they) do this. Plugin get their own fate_obs object. */
fate_obs fate_obs_create(uint32_t flags);
void     fate_obs_delete(fate_obs obs);
void     fate_obs_setmask(fate_obs obs, uint32_t mask);
uint32_t fate_obs_getmask(fate_obs obs);
/* mask should include a way to be notified at state execution commit time
 * instead of "as soon as it happens". 
 * How to handle various versions of a node across time ?
 * Resolved : Include the current tick in the event, and let the plugin
 * cope with it. */
fate_obs_watchd fate_obs_watch(fate_obs obs,
                               const uint8_t *path_no_wilcards, 
                               uint32_t mask);
void fate_obs_unwatch(fate_obs obs, fate_obs_watchd wh);
void fate_obs_broadcast(fate_obs obs, fate_obs* targets, size_t targets_count, 
                        fate_obs_event *e);


fate-plugin-<name>.dll

/* Custom part */
/* All functions except your event handler should be marked static. */

/* Required part */

const fate_obs_header fate_obs_header = {
    {1,0,0},
    {1,0,0},
    "<name>",
    "<name>, written by <author>. More info here as you wish.",
    my_handler
};
void my_handler(fate_obs_event *e) {
    static fate_obs myself = 0;
    switch(e->type) {
    case FATE_OBS_PLUGIN_LOADED:
        /*
         * This event is sent to a plugin right after it is loaded.
         *
         * This is where it all begins, and the engine has already decided 
         * if your plugin is compatible or not with it.
         * See e->plugin_loaded->obs; if obs is 0, then
         * it is not compatible.
         * A fate_obs may be as simple as an array index or a hash 
         * of its name and version.
         *
         * You are also given a shell object.
         * See e->plugin_loaded->sh;
         * 
         * If it is not compatible, here is your opportunity to alert the user,
         * because you will receive no more events (not even 
         * FATE_OBS_PLUGIN_UNLOADED) and get unloaded right after this 
         * function returns.
         * 
         * Otherwise, go on happily by initializing your plugin here, and
         * don't forget to set its event mask.
         * Telling the engine which events you want and don't want is important
         * because we care about in-game performance. 
         * By default, your event mask is only set to FATE_OBS_PLUGIN_UNLOAD
         * and FATE_OBS_SETMASK.
         */
        break;
    case FATE_OBS_PLUGIN_UNLOAD:
        /*
         * This event is sent to a plugin right before it is unloaded 
         * (given it was compatible with the engine).
         * Please be nice and clean-up your stuff here.
         */
        break;
    case FATE_OBS_ALL_PLUGINS_LOADED:
        /* 
         * The engine is done loading all the plugins it could find. 
         * Subscribe to this event if you want to interact with other 
         * plugins.
         * If the user adds new plugins to the directory at runtime, they will
         * be loaded and this event will be sent again.
         */
        break;
    case FATE_OBS_SETMASK:
        /*
         * The plugin's mask has been changed. Useful to know if another plugin
         * has done that behind our backs. The old value is given just in case. 
         * Otherwise, just ignore it.
         * 
         * Funnily, it's possible to set a mask such that we won't receive
         * FATE_OBS_SETMASK events.
         */
        break;
    case FATE_OBS_BROADCAST:
        /*
         * It is never actually sent by the engine, but it is there for events 
         * provided by plugins themselves. That way, plugins can 
         * communicate between them. 
         * One should use it like so :
         * 
         * fate_obs receiver; <- Of course, don't let it at NULL.
         * your_data_t your_data; <- Your own struct, for example.
         * fate_obs_event e;
         * e.type = FATE_OBS_BROADCAST;
         * e.broadcast = &your_data;
         * fate_obs_broadcast(myself, &receiver, 1, &e);
         *
         * Then the receiving plugin is responsible for handling the data in 
         * e.broadcast. You can put anything in it, like your plugin 
         * itself, the data's length, etc. However, it is your responsibility.
         */
        break;
    case FATE_OBS_NODE_LINKED:  /* After ++links_count or creation */
    case FATE_OBS_NODE_UNLINK: /* Before --links_count or deletion */
    case FATE_OBS_NODE_PRE_READ:  /* Before reading */
    case FATE_OBS_NODE_POST_READ:  /* After reading */
    case FATE_OBS_NODE_PRE_WRITE: /* Before writing (also provides new value) */
    case FATE_OBS_NODE_POST_WRITE: /* After writing (also provides old value) */
    case FATE_OBS_NODE_RENAME: /* Before renaming */
    case FATE_OBS_NODE_RENAMED: /* After renaming */
    case FATE_OBS_NODE_MOVE: /* Before moving */
    case FATE_OBS_NODE_MOVED: /* After moving */
    /* 
     * There is no need for more event types.
     * Feature that the engine is ready to expose to plugins are provided 
     * as nodes.
     * Otherwise, the plugin's developer is responsible for implementing the
     * nobsded features without disturbing the engine, which can be as
     * inconvenient as it sobsms.
     * FATE's goal being to do its best to be efficient, the line has to 
     * be drawn somewhere.
     *
     * Please note that you should use nodes to be notified of video/audio
     * backend changes. 
     * The user's not gonna be happy if you call a GL function whereas the
     * active video backend is Vulkan and there is no OpenGL context.
     * In the same way, it is your responsibility to make sure that the 
     * backend's functions you reach for are compatible with its version.
     * (i.e Don't call glShaderBinary() while the GL's version is 1.1, 
     * pretty please).
     * In such cases, your plugin should just do nothing instead.
     */
    }
}

