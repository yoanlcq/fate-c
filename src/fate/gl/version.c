#include <fate/gl/version.h>
#include <fate/log.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>

static const char *TAG = "fe_gl_version";

bool fe_gl_version_query(fe_gl_version *v) {
    /*
     * Might as well use
     *     glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
     * But it's only for desktop GL version >= 3.2.
     */
    int prof;
    int err = SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &prof);
    if(err < 0) {
        fe_logw(TAG, "Could not get profile mask: %s\n", SDL_GetError());
        return false;
    }
    v->core = v->compat = v->es = false;
    switch(prof) {
    case SDL_GL_CONTEXT_PROFILE_ES: v->es = true; break;
    case SDL_GL_CONTEXT_PROFILE_CORE: v->core = true; break;
    case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY: v->compat = true; break;
    }

    /* 
     * Basically, we parse a string because glGetIntegerv() with
     * GL_MAJOR_VERSION is not available on reasonably old GL versions.
     *
     * According to the OpenGL ES 2 reference :
     * "OpenGL ES <major>.<minor> <vendor-info>"
     * But on OpenGL ES 3 :
     * "<major>.<minor>"
     *
     * And on WebGL :
     * "WebGL <major>.<minor>"
     *
     * OpenGL wiki states that, for desktop GL < 3.0, you have to parse
     * the version string instead of using glGetIntegerv().
     */
    const char *version_s = (const char*) glGetString(GL_VERSION);
    if(!version_s) {
        fe_logw(TAG, "glGetString(GL_VERSION) returned NULL !\n");
        return false;
    }
    bool webgl = false;
    if(!strncmp(version_s, "OpenGL ES ", 10))
        v->es = true;
    if(!strncmp(version_s, "WebGL ", 6)) {
        v->es = true;
        webgl = true;
    }
    while(!isdigit(*version_s)) 
        ++version_s;
    v->major = strtoul(version_s, NULL, 10);
    v->minor = strtoul(strchr(version_s, '.')+1, NULL, 10);
    if(webgl)
        ++(v->major);
    return true;
}


