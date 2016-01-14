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
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "cube.h"


struct cube_main {
    bool running;
    SDL_Window *window;
    SDL_DisplayMode current_display_mode;
    SDL_GLContext glctx;
    uint16_t win_w, win_h;
    uint16_t old_win_w, old_win_h;
    Cube cube;
    float distance;
    vec3 eye, center, up;
    GLint MVPMatrixLoc;
    GLint ufInvertLoc;
    mat4 MVPMatrix, Projection, View, Model;
    int mousex, mousey;
    bool mousein, mousedown;
    float h_angle, v_angle;
    float R_x;
    float R_y;
    bool zoom_in, zoom_out;
    bool go_west, go_east;
    float splitx;
    unsigned fps_ceil;
    unsigned framerate_limit;
    double frameno;
    uint32_t lim_last_time;
    uint32_t last_time;
};

void cube_main_init(struct cube_main *m) {

    m->running = true;

#if SDL_MAJOR_VERSION > 2 \
 || (SDL_MAJOR_VERSION==2 && SDL_MINOR_VERSION>0) \
 || (SDL_MAJOR_VERSION==2 && SDL_MINOR_VERSION==0 && SDL_PATCHLEVEL>=4)
#if defined(FATE_LINUX) || defined(FATE_FREEBSD) || defined(FATE_OSX)
    SDL_SetHintWithPriority("SDL_HINT_NO_SIGNAL_HANDLERS", "1", 
                            SDL_HINT_OVERRIDE);
#endif
#endif
    puts("Initializing SDL...");
    if(SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER|SDL_INIT_HAPTIC)) < 0)
        fate_fatal("SDL_Init failed: %s\n", SDL_GetError());

    puts("Initializing log system and global state...");
    //We do this after SDL_Init to (hopefully) override its signal handler.
    fate_globalstate_init(fate_gs);

    fate_logf("Welcome to F.A.T.E !\n"
              "Version : %d.%d.%d (\"%s\")\n"
              "Compiled on %s, %s.\n\n",
              FATE_VERSION_MAJOR, FATE_VERSION_MINOR, FATE_VERSION_PATCH,
              FATE_VERSION_CODENAME,
              __DATE__, __TIME__);

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    fate_logf("--- SDL version ---\n"
              "    Compiled : %d.%d.%d (revision %s)\n"
              "    Linked   : %d.%d.%d (revision %s)\n\n",
              compiled.major, compiled.minor, compiled.patch, SDL_REVISION,
              linked.major, linked.minor, linked.patch, SDL_GetRevision());

    fate_logf_video("--- SDL Video drivers ---\n");
    int ndrivers = SDL_GetNumVideoDrivers();
    for(--ndrivers ; ndrivers>=0 ; --ndrivers)
        fate_logf_video("    %s\n", SDL_GetVideoDriver(ndrivers));
    fate_logf_video("\n");
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 
                        0//SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
#ifdef FATE_GL_DEBUG
                       |SDL_GL_CONTEXT_DEBUG_FLAG
#endif
                        );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
                        SDL_GL_CONTEXT_PROFILE_CORE);
/*
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
*/
    m->win_w = 640;
    m->win_h = 480;
    m->window = SDL_CreateWindow("F.A.T.E Cube demo", 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m->win_w, m->win_h,
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);

    if(!m->window)
        fate_fatal("SDL_CreateWindow failed: %s\n", SDL_GetError());

    if(SDL_GetCurrentDisplayMode(0, &m->current_display_mode))
        fate_fatal("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());

    m->glctx = SDL_GL_CreateContext(m->window);
    if(!m->glctx)
        fate_fatal("SDL_GL_CreateContext failed : %s\n", SDL_GetError());

    glewExperimental = GL_TRUE;
    fate_logf_video("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    GLenum glew = glewInit();
    if(glew != GLEW_OK)
        fate_fatal("Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glew));

    fate_logf_video(
            "--- OpenGL version ---\n"
            "    Required  : >= %d.%d\n"
            "    Supported :    %s\n"
            "\n"
            "--- OpenGL device ---\n"
            "    Renderer  : %s\n"
            "    Vendor    : %s\n"
            "\n",
            4, 1, 
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

    GLint gl_major, gl_minor;
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
    if(gl_major < 4 || (gl_major == 4 && gl_minor < 1))
        fate_fatal("The OpenGL version reported by your driver is "
                   "not supported yet.\nSorry. I'm working on it.\n");
    GLint ctxflags, ctxpflags, depth_bits, stencil_bits;
    GLboolean double_buffer, stereo_buffers;

    glGetIntegerv(GL_CONTEXT_FLAGS, &ctxflags);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, 
            GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, 
            GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
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
#ifdef FATE_EMSCRIPTEN
        "", "",
#else
        ctxflags & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ? "robust_access " : "",
        ctxflags & GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR ? "no_error " : "",
#endif
        ctxflags,
        double_buffer ? "yes" : "no",
        stereo_buffers ? "yes" : "no",
        depth_bits,
        stencil_bits
    );

    GLint num_glexts, i;
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
        fate_fatal("Could not build the OpenGL program.\n");
    fate_gl_mkprog_cleanup();
    fate_glObjectLabel(GL_PROGRAM, progid, -1, "\"Cube program\"");
    Cube_init(&m->cube, progid);

    glEnable(GL_PRIMITIVE_RESTART); /* Since OpenGL 3.1 */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    m->distance = 6.0f;
    m->eye[0] = 0.0f;
    m->eye[1] = 0.0f;
    m->eye[2] = -m->distance;
    memset(m->center, 0, sizeof(vec3));
    m->up[0] = 0.0f;
    m->up[1] = 1.0f;
    m->up[2] = 0.0f;
    m->MVPMatrixLoc = glGetUniformLocation(m->cube.prog, "MVPMatrix");
    m->ufInvertLoc  = glGetUniformLocation(m->cube.prog, "ufInvert");

#define UPDATE_VIEW() \
    mat4_identity(m->Model); \
    mat4_look_at(m->View, m->eye, m->center, m->up)

#define UPDATE_MVP() \
    mat4_mul(m->MVPMatrix, m->Projection, m->View); \
    mat4_mul(m->MVPMatrix, m->Model, m->MVPMatrix); \
    glUniformMatrix4fv(m->MVPMatrixLoc, 1, GL_FALSE, &m->MVPMatrix[0][0])
   
#define FATE_FALLBACK_REFRESH_RATE 60
#define FATE_DEFAULT_FPS_CEIL 256
#define FATE_DEFAULT_NEAR 0.25
#define FATE_DEFAULT_FAR  1024.0
#define FATE_DEFAULT_FOV  (75.0*M_PI/180.0)

#define RESIZE(_W_,_H_) \
    glViewport(0, 0, _W_, _H_); \
    m->win_w = _W_; m->win_h = _H_; \
    mat4_perspective(m->Projection, FATE_DEFAULT_FOV, _W_/(float)_H_, \
                     FATE_DEFAULT_NEAR, FATE_DEFAULT_FAR); \
    UPDATE_MVP()

    UPDATE_VIEW();
    RESIZE(m->win_w, m->win_h);

    m->mousex = m->mousey = 0;
    m->mousein = m->mousedown = false;
    m->h_angle = m->v_angle = 0.0f;
    m->R_x = m->R_y = 0.0f;
    m->zoom_in = m->zoom_out = false;

    m->go_west = m->go_east = false;
    m->splitx = 0.5f;

    m->fps_ceil = FATE_DEFAULT_FPS_CEIL;
    m->framerate_limit = 0;

    if(m->framerate_limit <= 0)
        if(SDL_GL_SetSwapInterval(1) < 0)
            fate_logf_err("Warning : Vsync is disabled. The FPS may skyrocket.\n");

    m->frameno = 0;
    m->lim_last_time = SDL_GetTicks();
    m->last_time = SDL_GetTicks();
}




void cube_main_loop_iteration(void *arg) {
    struct cube_main *m = arg;

    if(!m->running) {
        glDeleteProgram(m->cube.prog);
        Cube_free(&m->cube);
        fate_globalstate_deinit(fate_gs);
        SDL_GL_DeleteContext(m->glctx);
        SDL_DestroyWindow(m->window);
        SDL_Quit();
        exit(EXIT_SUCCESS);
    }

    uint32_t current_time;
    const double fps_counter_interval = 1000.0; /* Should be in [100, 1000] */
    bool dirty = false;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
            case SDL_APP_TERMINATING: 
                m->running = false; 
                break;
            case SDL_APP_LOWMEMORY: break;
            case SDL_APP_WILLENTERBACKGROUND: break;
            case SDL_APP_DIDENTERBACKGROUND: break;
            case SDL_APP_WILLENTERFOREGROUND: break;
            case SDL_APP_DIDENTERFOREGROUND: break;
            case SDL_WINDOWEVENT: 
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    m->running = false;
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
                case SDL_WINDOWEVENT_ENTER: m->mousein = true;  break;
                case SDL_WINDOWEVENT_LEAVE: m->mousein = false; break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:break;
                case SDL_WINDOWEVENT_FOCUS_LOST:break;
                }
                break;
            case SDL_SYSWMEVENT: break;

            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:     m->go_west  = true; break;
                    case SDLK_RIGHT:    m->go_east  = true; break;
                    case SDLK_KP_PLUS:  m->zoom_in  = true; break;
                    case SDLK_KP_MINUS: m->zoom_out = true; break;
                }
                break;
            case SDL_KEYUP: 
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:     m->go_west  = false; break;
                    case SDLK_RIGHT:    m->go_east  = false; break;
                    case SDLK_KP_PLUS:  m->zoom_in  = false; break;
                    case SDLK_KP_MINUS: m->zoom_out = false; break;
                    case SDLK_F11: 
                    {
                        bool is_fullscreen = !!(SDL_GetWindowFlags(m->window) & SDL_WINDOW_FULLSCREEN);
                        if(!is_fullscreen) {
                            m->old_win_w = m->win_w;
                            m->old_win_h = m->win_h;
                            SDL_SetWindowSize(m->window, 
                                    m->current_display_mode.w, 
                                    m->current_display_mode.h);
                            fate_logf_video("Now using %s mode.\n", "fullscreen");
                        }
                        if(SDL_SetWindowFullscreen(m->window, 
                                (!is_fullscreen)*SDL_WINDOW_FULLSCREEN)<0) {
                            fate_logf_video("Failed to toggle fullscreen : %s\n", SDL_GetError());
                            break;
                        }
                        if(is_fullscreen) {
                            SDL_SetWindowSize(m->window, m->old_win_w, m->old_win_h);
                            SDL_SetWindowPosition(m->window, 
                                    SDL_WINDOWPOS_CENTERED, 
                                    SDL_WINDOWPOS_CENTERED);
                            fate_logf_video("Now using %s mode.\n", "windowed");
                        }
                    }
                        break;
                }
                break;
            case SDL_TEXTEDITING: break;
            case SDL_TEXTINPUT: break;
            case SDL_MOUSEMOTION: 
                
                if(!m->mousein) {
                    m->mousex = event.motion.x;
                    m->mousey = event.motion.y;
                    m->mousein = true;
                }
                if(m->mousedown) {
                    m->h_angle += (event.motion.x - m->mousex)*M_PI/180.0f;
                    m->v_angle += (event.motion.y - m->mousey)*M_PI/180.0f;
                    if(m->v_angle >= M_PI/2)
                        m->v_angle = M_PI/2 - M_PI/180.0f;
                    else if(m->v_angle <= -M_PI/2)
                        m->v_angle = -M_PI/2 + M_PI/180.0f;
                    m->eye[0] =  m->distance*sinf(m->h_angle)*cosf(m->v_angle);
                    m->eye[1] =  m->distance*sinf(m->v_angle);
                    m->eye[2] = -m->distance*cosf(m->h_angle)*cosf(m->v_angle);

                    UPDATE_VIEW();
                    UPDATE_MVP();
                }    
                m->mousex = event.motion.x;
                m->mousey = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN: 
                m->mousedown = true;                  
                break;
            case SDL_MOUSEBUTTONUP: 
                m->mousedown = false;
                break;
            case SDL_MOUSEWHEEL: 
                m->distance -= event.wheel.y;
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

    if(m->go_west && m->splitx > 0.0025f)
        m->splitx -= 0.0025f;
    if(m->go_east && m->splitx < 0.9975f)
        m->splitx += 0.0025f;
    if(m->R_x > 10.0f || m->R_x < -10.0f || m->R_y>10.0f || m->R_y<-10.0f) {
        m->eye[0] += m->R_x/200.0f;
        m->eye[1] += m->R_y/200.0f;
        dirty = true;
    }
    if(m->zoom_in) {
        m->distance -= 0.25f;
        dirty = true;
    } else if(m->zoom_out) {
        m->distance += 0.25f;
        dirty = true;
    }
    if(dirty) {
        m->eye[0] =  m->distance*sinf(m->h_angle)*cosf(m->v_angle);
        m->eye[1] =  m->distance*sinf(m->v_angle);
        m->eye[2] = -m->distance*cosf(m->h_angle)*cosf(m->v_angle);
        UPDATE_VIEW();
        UPDATE_MVP();
        dirty = false;
    }

    /* See http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/ */
    current_time = SDL_GetTicks();
    ++(m->frameno);
    if(SDL_TICKS_PASSED(current_time, m->last_time+fps_counter_interval))
    {
        unsigned fps = lround(m->frameno*1000.0/fps_counter_interval);
        fate_logf_video("%u frames under %lg milliseconds = "
                        "%lg milliseconds/frame = "
                        "%u FPS\n", 
                (unsigned)m->frameno,
                fps_counter_interval,
                fps_counter_interval/m->frameno, 
                fps);
        m->frameno = 0;
        m->last_time += fps_counter_interval;
        if(m->framerate_limit <= 0 && fps > m->fps_ceil) {
            const char *str;
            if(m->current_display_mode.refresh_rate) {
                m->framerate_limit = m->current_display_mode.refresh_rate;
                str = "from display mode info";
            } else {
                m->framerate_limit = FATE_FALLBACK_REFRESH_RATE;
                str = "fallback";
            }
            fate_logf_video("Abnormal FPS detected (Vsync is not working). "
                            "Now limiting FPS to %u (%s).\n", 
                            m->framerate_limit, str);
        }
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    glScissor(0, 0, m->splitx*(float)m->win_w, m->win_h);
    glClearColor(0.3f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1ui(m->ufInvertLoc, 0);
    Cube_draw(&m->cube);

    glScissor(m->splitx*(float)m->win_w, 0, m->win_w-m->splitx*(float)m->win_w, m->win_h);
    glClearColor(0.7f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1ui(m->ufInvertLoc, 1);
    Cube_draw(&m->cube);
    
    glDisable(GL_SCISSOR_TEST); 

    if(m->framerate_limit > 0) {
        current_time = SDL_GetTicks();
#define a_frame (1000/m->framerate_limit)
        if(current_time-m->lim_last_time < a_frame)
            SDL_Delay(a_frame-(current_time-m->lim_last_time));
#undef a_frame
        m->lim_last_time = SDL_GetTicks();
    }

    SDL_GL_SwapWindow(m->window);
}


#ifdef FATE_WINDOWS
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   PSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
    struct cube_main m;
    cube_main_init(&m);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(cube_main_loop_iteration, &m, 0, true);
#else
    for(;;)
        cube_main_loop_iteration(&m);
#endif
    return EXIT_SUCCESS;
}

