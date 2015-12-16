#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <fate.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_revision.h>
#include <linmath/linmath.h>
#include "cube.h"

#if defined(FATE_DEFS_WINDOWS)
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   PSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
#if SDL_MAJOR_VERSION > 2 \
 || (SDL_MAJOR_VERSION==2 && SDL_MINOR_VERSION>0) \
 || (SDL_MAJOR_VERSION==2 && SDL_MINOR_VERSION==0 && SDL_PATCHLEVEL>=4)
#if defined(FATE_DEFS_LINUX)  \
|| defined(FATE_DEFS_FREEBSD) \
|| defined(FATE_DEFS_OSX)
    SDL_SetHintWithpriority("SDL_HINT_NO_SIGNAL_HANDLERS", "1", 
                            SDL_HINT_OVERRIDE);
#endif
#endif

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fate_logf_err("SDL_Init failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //We do this after SDL_Init to (hopefully) override its signal handler.
    fate_globalstate_init(fate_gs);

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    fate_logf("--- SDL version ---\n"
              "    Compiled : %d.%d.%d (revision %s)\n"
              "    Linked   : %d.%d.%d (revision %s)\n\n",
              compiled.major, compiled.minor, compiled.patch, SDL_REVISION,
              linked.major, linked.minor, linked.patch, SDL_GetRevision());


    char *game_path = fate_sys_getgamepath();
    if(!game_path) {
        fate_logf_err("Couldn't find the game's path.\nSorry.\n");
        exit(EXIT_FAILURE);
    }
    fate_sys_set_current_directory(game_path);
    free(game_path);

    fate_logf_video("--- Video drivers ---\n");
    int ndrivers = SDL_GetNumVideoDrivers();
    for(--ndrivers ; ndrivers>=0 ; --ndrivers)
        fate_logf_video("    %s\n", SDL_GetVideoDriver(ndrivers));
    fate_logf_video("\n");

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifdef FATE_GL_DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);


    uint16_t win_w = 640, win_h = 480;
    SDL_Window *window = SDL_CreateWindow("F.A.T.E Cube demo", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h,
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);

    if(!window) {
        fate_logf_err("SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    if(!ctx) {
        fate_logf_err("SDL_GL_CreateContext failed : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    //SDL_GL_MakeCurrent(window, ctx);

    fate_logf_video(
            "--- OpenGL version ---\n"
            "    Required  : >= %d.%d\n"
            "    Supported :    %s\n"
            "\n"
            "--- OpenGL device ---\n"
            "    Renderer  : %s\n"
            "    Vendor    : %s\n"
            "\n",
            4, 3, 
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

    GLint gl_major, gl_minor;
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);

    if(gl_major < 4 || (gl_major == 4 && gl_minor < 3))
    {
        fate_logf_err("The OpenGL version reported by your driver is "
                      "not supported yet.\nSorry. I'm working on it.\n");
        exit(EXIT_FAILURE);
    }

    glewExperimental = GL_TRUE;
    fate_logf_video("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    GLenum glew = glewInit();
    if(glew != GLEW_OK)
    {
        fate_logf_video("Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glew));
        exit(EXIT_FAILURE);
    }


    GLint ctxflags, ctxpflags, depth_bits, stencil_bits;
    GLboolean double_buffer, stereo_buffers;

    glGetIntegerv(GL_CONTEXT_FLAGS, &ctxflags);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, 
            GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, 
            GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
    glGetIntegerv(GL_STENCIL_BITS, &stencil_bits);
    glGetBooleanv(GL_DOUBLEBUFFER, &double_buffer);
    glGetBooleanv(GL_STEREO, &stereo_buffers);

    fate_logf_video(
        "--- Active OpenGL context settings ---\n"
        "    Version             : %d.%d\n"
        "    GLSL version        : %s\n"
        "    Profile flags       : %s%s(%#x)\n"
        "    Context flags       : %s%s%s%s(%#x)\n"
        "    Double buffering    : %s\n"
        "    Stereo buffers      : %s\n"
        "    Depth buffer bits   : %d\n"
        "    Stencil buffer bits : %d\n",
        gl_major, gl_minor,
        glGetString(GL_SHADING_LANGUAGE_VERSION),
        ctxpflags & GL_CONTEXT_CORE_PROFILE_BIT ? "core " : "",
        ctxpflags & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT ? "compatibility " :"",
        ctxpflags,
        ctxflags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ? "forward_compatible " : "",
        ctxflags & GL_CONTEXT_FLAG_DEBUG_BIT ? "debug " : "",
        ctxflags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ? "robust_access " : "",
        ctxflags & GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR ? "no_error " : "",
        ctxflags,
        double_buffer ? "yes" : "no",
        stereo_buffers ? "yes" : "no",
        depth_bits,
        stencil_bits
    );

    int num_glexts, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_glexts);
    fate_logf_video("    Extensions :\n");
    for(i=0 ; i<num_glexts ; i++) {
        fate_logf_video("%-38s", glGetStringi(GL_EXTENSIONS, i));
        if(i+1<num_glexts) {
            ++i;
            fate_logf_video(" %-38s", glGetStringi(GL_EXTENSIONS, i));
        }
        fate_logf_video("\n");
    }
    fate_logf_video("\n");

    fate_gl_debug_setup(gl_major, gl_minor, true);

    fate_gl_mkprog_setup(gl_major, gl_minor);
    GLuint progid = glCreateProgram();
    if(!fate_gl_mkprog(progid,
                       "data/OpenGL/triangles.glpb",
                       "res/shaders/triangles.330.core.vert",
                       "res/shaders/triangles.330.core.frag",
                       NULL))
    {
        fate_logf_err("Can't continue.\n");
        exit(EXIT_FAILURE);
    }
    fate_gl_mkprog_cleanup();
    fate_glObjectLabel(GL_PROGRAM, progid, -1, "\"Cube program\"");
    Cube cube;
    Cube_init(&cube, progid);

    glEnable(GL_PRIMITIVE_RESTART); /* Since OpenGL 3.1 */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    float distance = 6.0f;
    vec3 eye = {0.0f, 0.0f, -distance}, center = {0,0,0}, up = {0,1,0};
    GLint MVPMatrixLoc = glGetUniformLocation(progid, "MVPMatrix");
    GLint ufInvertLoc  = glGetUniformLocation(progid, "ufInvert");
    mat4x4 MVPMatrix, Projection, View, Model;

#define UPDATE_VIEW() \
    mat4x4_identity(Model); \
    mat4x4_look_at(View, eye, center, up);

#define UPDATE_MVP() \
    mat4x4_dup(MVPMatrix, Model); \
    mat4x4_mul(MVPMatrix, View, MVPMatrix); \
    mat4x4_mul(MVPMatrix, Projection, MVPMatrix); \
    glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0])
    
#define RESIZE(_W_,_H_) \
    glViewport(0, 0, _W_, _H_); \
    win_w = _W_; win_h = _H_; \
    mat4x4_perspective(Projection, 45, _W_/(float)_H_, 1.0f, 100.0f); \
    UPDATE_MVP();

    UPDATE_VIEW();
    RESIZE(win_w, win_h);

    int mousex=0, mousey=0;
    bool mousein = false, mousedown = false;
    float h_angle = 0.0f, v_angle = 0.0f;
    float L_x = 0.0f;
    float L_y = 0.0f;
    float R_x = 0.0f;
    float R_y = 0.0f;
    bool zoom_in  = false;
    bool zoom_out = false;

    bool go_west = false, go_east = false;
    float splitx = 0.5f;
    bool running = true;
    bool dirty = false;

    if(SDL_GL_SetSwapInterval(1) < 0)
        fate_logf_err("Warning : Vsync is disabled. The FPS may skyrocket.\n");

    uint16_t old_win_w, old_win_h;
    double frameno = 0;
    const double fps_counter_interval = 100.0; /* Should be in [100, 1000] */
    uint32_t current_time, last_time = SDL_GetTicks();

    while(running) {

        /* See http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/ */
        current_time = SDL_GetTicks();
        ++frameno;
        if(SDL_TICKS_PASSED(current_time, last_time+fps_counter_interval))
        {
            fate_logf_video("%lf milliseconds/frame = %ld FPS\n", 
                    fps_counter_interval/frameno, lround(frameno*1000.0/fps_counter_interval));
            frameno = 0;
            last_time += fps_counter_interval;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);

        glScissor(0, 0, splitx*(float)win_w, win_h);
        glClearColor(0.3f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1ui(ufInvertLoc, 0);
        Cube_draw(&cube);

        glScissor(splitx*(float)win_w, 0, win_w-splitx*(float)win_w, win_h);
        glClearColor(0.7f, 0.1f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1ui(ufInvertLoc, 1);
        Cube_draw(&cube);
        
        glDisable(GL_SCISSOR_TEST); 

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        char *axis_str;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                case SDL_APP_TERMINATING: 
                    running = false; 
                    break;
                case SDL_APP_LOWMEMORY: break;
                case SDL_APP_WILLENTERBACKGROUND: break;
                case SDL_APP_DIDENTERBACKGROUND: break;
                case SDL_APP_WILLENTERFOREGROUND: break;
                case SDL_APP_DIDENTERFOREGROUND: break;
                case SDL_WINDOWEVENT: 
                    switch (event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        running = false;
                        break;
                    case SDL_WINDOWEVENT_SHOWN:break;
                    case SDL_WINDOWEVENT_HIDDEN:break;
                    case SDL_WINDOWEVENT_EXPOSED:break;
                    case SDL_WINDOWEVENT_MOVED:break;
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        RESIZE(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:break;
                    case SDL_WINDOWEVENT_MAXIMIZED:break;
                    case SDL_WINDOWEVENT_RESTORED:break;
                    case SDL_WINDOWEVENT_ENTER: mousein = true;  break;
                    case SDL_WINDOWEVENT_LEAVE: mousein = false; break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:break;
                    }
                    break;
                case SDL_SYSWMEVENT: break;

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_LEFT:  go_west  = true; break;
                        case SDLK_RIGHT: go_east  = true; break;
                        case SDLK_KP_PLUS: zoom_in  = true; break;
                        case SDLK_KP_MINUS: zoom_out = true; break;
                    }
                    break;
                case SDL_KEYUP: 
                    switch(event.key.keysym.sym) {
                        case SDLK_LEFT:     go_west  = false; break;
                        case SDLK_RIGHT:    go_east  = false; break;
                        case SDLK_KP_PLUS:  zoom_in  = false; break;
                        case SDLK_KP_MINUS: zoom_out = false; break;
                        case SDLK_F11: 
                        {
                            bool is_fullscreen = !!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
                            if(!is_fullscreen) {
                                old_win_w = win_w;
                                old_win_h = win_h;
                                SDL_DisplayMode dm;
                                if(SDL_GetCurrentDisplayMode(0, &dm) != 0) {
                                    fate_logf_video("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
                                    break;
                                }
                                SDL_SetWindowSize(window, dm.w, dm.h);
                            }
                            if(SDL_SetWindowFullscreen(window, 
                                    (!is_fullscreen)*SDL_WINDOW_FULLSCREEN)<0) {
                                fate_logf_video("Failed to toggle fullscreen : %s\n", SDL_GetError());
                                break;
                            }
                            if(is_fullscreen) {
                                SDL_SetWindowSize(window, old_win_w, old_win_h);
                                SDL_SetWindowPosition(window, 
                                        SDL_WINDOWPOS_CENTERED, 
                                        SDL_WINDOWPOS_CENTERED);
                            }
                        }
                            break;
                    }
                    break;
                case SDL_TEXTEDITING: break;
                case SDL_TEXTINPUT: break;
                case SDL_MOUSEMOTION: 
                    
                    if(!mousein) {
                        mousex = event.motion.x;
                        mousey = event.motion.y;
                        mousein = true;
                    }
                    if(mousedown) {
                        h_angle += (event.motion.x - mousex)*M_PI/180.0f;
                        v_angle += (event.motion.y - mousey)*M_PI/180.0f;
                        if(v_angle >= M_PI/2)
                            v_angle = M_PI/2 - M_PI/180.0f;
                        else if(v_angle <= -M_PI/2)
                            v_angle = -M_PI/2 + M_PI/180.0f;
                        eye[0] =  distance*sinf(h_angle)*cosf(v_angle);
                        eye[1] =  distance*sinf(v_angle);
                        eye[2] = -distance*cosf(h_angle)*cosf(v_angle);

                        UPDATE_VIEW();
                        UPDATE_MVP();
                    }    
                    mousex = event.motion.x;
                    mousey = event.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN: 
                    mousedown = true;                  
                    break;
                case SDL_MOUSEBUTTONUP: 
                    mousedown = false;
                    break;
                case SDL_MOUSEWHEEL: 
                    distance -= event.wheel.y;
                    dirty = true;
                    break;
                case SDL_JOYAXISMOTION: break;
                case SDL_JOYBALLMOTION: break;
                case SDL_JOYHATMOTION: break;
                case SDL_JOYBUTTONDOWN: break;
                case SDL_JOYBUTTONUP: break;
                case SDL_JOYDEVICEADDED: break;
                case SDL_JOYDEVICEREMOVED: break;
                case SDL_CONTROLLERAXISMOTION: break;
                case SDL_CONTROLLERBUTTONDOWN: break;
                case SDL_CONTROLLERBUTTONUP: break;
                case SDL_CONTROLLERDEVICEADDED: break;
                case SDL_CONTROLLERDEVICEREMOVED: break;
                case SDL_CONTROLLERDEVICEREMAPPED: break;
                case SDL_FINGERDOWN: break;
                case SDL_FINGERUP: break;
                case SDL_FINGERMOTION: break;
                case SDL_DOLLARGESTURE: break;
                case SDL_DOLLARRECORD: break;
                case SDL_MULTIGESTURE: break;
                case SDL_CLIPBOARDUPDATE: break;
                case SDL_DROPFILE: break;
             /* case SDL_AUDIODEVICEADDED: break;
                case SDL_AUDIODEVICEREMOVED: break; */
                case SDL_RENDER_TARGETS_RESET: break;
                /* case SDL_RENDER_DEVICE_RESET: break; */
                case SDL_USEREVENT: break;
            } /* end switch(event.type) */
        } /* end while(PollEvent(window)) */

        if(go_west && splitx > 0.0025f)
            splitx -= 0.0025f;
        if(go_east && splitx < 0.9975f)
            splitx += 0.0025f;
        if(R_x > 10.0f || R_x < -10.0f || R_y>10.0f || R_y<-10.0f) {
            eye[0] += R_x/200.0f;
            eye[1] += R_y/200.0f;
            dirty = true;
        }
        if(zoom_in) {
            distance -= 0.25f;
            dirty = true;
        } else if(zoom_out) {
            distance += 0.25f;
            dirty = true;
        }
        if(dirty) {
            eye[0] =  distance*sinf(h_angle)*cosf(v_angle);
            eye[1] =  distance*sinf(v_angle);
            eye[2] = -distance*cosf(h_angle)*cosf(v_angle);
            UPDATE_VIEW();
            UPDATE_MVP();
            dirty = false;
        }
    } /* end while(running) */

    Cube_free(&cube);
    glDeleteProgram(progid);
    fate_globalstate_deinit(fate_gs);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}
