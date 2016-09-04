#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <fate/fate.h>
#ifdef __MINGW64_VERSION_MAJOR
#undef __MINGW64_VERSION_MAJOR
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_revision.h>
#include <linmath/linmath.h>
#ifdef FE_TARGET_EMSCRIPTEN
#include <emscripten.h>
#endif
#include "cube.h"

#define FE_GAME_NAME "Rainbow Dice"
#define FE_GAME_ID "Rainbow_Dice"

struct cube_main {
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

static const char *TAG = "cube_main";

void cube_main_init(struct cube_main *m) {

#if SDL_VERSION_ATLEAST(2, 0, 4)
    SDL_SetHintWithPriority("SDL_HINT_NO_SIGNAL_HANDLERS", "1", 
                            SDL_HINT_OVERRIDE);
#endif

    fe_logi(TAG, "Initializing SDL...");
    if(SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER|SDL_INIT_HAPTIC)) < 0)
        fe_fatal(TAG, "SDL_Init failed: %s\n", SDL_GetError());

    fe_logi(TAG, "Initializing log system and global state...");
    //We do this after SDL_Init to (hopefully) override its signal handler.
    fe_globalstate_init(fe_gs);

    fe_logi(TAG, "Welcome to F.A.T.E !\n"
              "Version : %d.%d.%d (\"%s\")\n"
              "Compiled on %s, %s.\n"
              "Platform : %s\n\n",
              FE_VERSION_MAJOR, FE_VERSION_MINOR, FE_VERSION_PATCH,
              FE_VERSION_CODENAME,
              __DATE__, __TIME__,
#ifdef FE_TARGET_EMSCRIPTEN
              "Emscripten"
#else
              SDL_GetPlatform()
#endif
              );

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    fe_logi(TAG, "--- SDL version ---\n"
              "    Compiled : %d.%d.%d (revision %s)\n"
              "    Linked   : %d.%d.%d (revision %s)\n\n",
              compiled.major, compiled.minor, compiled.patch, SDL_REVISION,
              linked.major, linked.minor, linked.patch, SDL_GetRevision());

    fe_logi(TAG, "--- SDL Video drivers ---\n");
    int ndrivers = SDL_GetNumVideoDrivers();
    for(--ndrivers ; ndrivers>=0 ; --ndrivers)
        fe_logi(TAG, "    %s\n", SDL_GetVideoDriver(ndrivers));
    fe_logi(TAG, "\n");
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 
                        0//SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
#ifdef FE_GL_DBG
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
    m->window = SDL_CreateWindow(FE_GAME_NAME, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m->win_w, m->win_h,
            SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);

    if(!m->window)
        fe_fatal(TAG, "SDL_CreateWindow failed: %s\n", SDL_GetError());

    if(SDL_GetCurrentDisplayMode(0, &m->current_display_mode))
        fe_fatal(TAG, "SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());

    m->glctx = SDL_GL_CreateContext(m->window);
    if(!m->glctx)
        fe_fatal(TAG, "SDL_GL_CreateContext failed : %s\n", SDL_GetError());

    glewExperimental = GL_TRUE;
    fe_logi(TAG, "Using GLEW %s\n", glewGetString(GLEW_VERSION));
    GLenum glew = glewInit();
    if(glew != GLEW_OK)
        fe_fatal(TAG, "Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glew));
    glGetError();

    fe_logi(TAG,
            "--- OpenGL version ---\n"
            "    Required  : >= %d.%d\n"
            "    Supported :    %s\n"
            "\n"
            "--- OpenGL device ---\n"
            "    Renderer  : %s\n"
            "    Vendor    : %s\n"
            "\n",
            3, 0, 
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

    fe_gl_version gl_version;
    if(!fe_gl_version_query(&gl_version))
        fe_fatal(TAG, "Could not parse the OpenGL version and profile mask.\n");
    {
        unsigned maj, min, es;
        maj = gl_version.major;
        min = gl_version.minor;
        es  = gl_version.es;
        fe_logi(TAG, "Parsed OpenGL version : %u.%u%s\n", maj, min, es ? "ES" : "");
        bool supported = (es ? maj>=2 : maj>=3);
        if(!supported)
            fe_fatal(TAG, "The OpenGL version reported by your driver is "
                       "not supported yet.\nSorry. I'm working on it.\n");
    }
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

    fe_logi(TAG,
        "--- Active OpenGL context settings ---\n"
        "    Version             : %d.%d\n"
        "    GLSL version        : %s\n"
        "    Profile flags       : %s%s(%#x)\n"
        "    Context flags       : %s%s%s%s(%#x)\n"
        "    Double buffering    : %s\n"
        "    Stereo buffers      : %s\n"
        "    Depth buffer bits   : %d\n"
        "    Stencil buffer bits : %d\n",
        gl_version.major, gl_version.minor,
        glGetString(GL_SHADING_LANGUAGE_VERSION),
        ctxpflags & GL_CONTEXT_CORE_PROFILE_BIT ? "core " : "",
        ctxpflags & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT ? "compatibility " :"",
        ctxpflags,
        ctxflags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ? "forward_compatible " : "",
        ctxflags & GL_CONTEXT_FLAG_DEBUG_BIT ? "debug " : "",
#ifdef FE_TARGET_EMSCRIPTEN
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

#ifndef FE_TARGET_EMSCRIPTEN /* glGetStringi(): unresolved symbol */
    GLint num_glexts, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_glexts);
    fe_logi(TAG, "    Extensions :\n");
    for(i=0 ; i<num_glexts ; i++) {
        fe_logi(TAG, "%-38s", glGetStringi(GL_EXTENSIONS, i));
        if(i+1<num_glexts) {
            ++i;
            fe_logi(TAG, " %-38s", glGetStringi(GL_EXTENSIONS, i));
        }
        fe_logi(TAG, "\n");
    }
    fe_logi(TAG, "\n");
#endif

    fe_gl_dbg_setup(&gl_version, true);
    fe_gl_mkprog_setup(&gl_version);
    GLuint progid = glCreateProgram();
    fe_gl_shader_source_set ss = {{0}};
    ss.vert.base = fe_gl_src_tri_130_vert;
    ss.frag.base = fe_gl_src_tri_130_frag;
    ss.before_linking = fe_gl_src_before_linking;
    if(!fe_gl_mkprog_no_binary(progid, &ss))
        fe_fatal(TAG, "Could not build the OpenGL program!\n"
		"More details on this error have been logged.\n");
    fe_gl_mkprog_cleanup();
    fe_gl_dbg_glObjectLabel(GL_PROGRAM, progid, -1, "\"Cube program\"");
    Cube_init(&m->cube, progid);

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
    m->MVPMatrixLoc = glGetUniformLocation(m->cube.prog, "u_mvp");
    m->ufInvertLoc  = glGetUniformLocation(m->cube.prog, "u_invert");

#define UPDATE_VIEW() \
    mat4_identity(m->Model); \
    mat4_look_at(m->View, m->eye, m->center, m->up)

#define UPDATE_MVP() \
    mat4_mul(m->MVPMatrix, m->Projection, m->View); \
    mat4_mul(m->MVPMatrix, m->Model, m->MVPMatrix); \
    glUniformMatrix4fv(m->MVPMatrixLoc, 1, GL_FALSE, &m->MVPMatrix[0][0])
   
#define FE_FALLBACK_REFRESH_RATE 60
#define FE_DEFAULT_FPS_CEIL 256
#define FE_DEFAULT_NEAR 0.25
#define FE_DEFAULT_FAR  1024.0
#define FE_DEFAULT_FOV  (75.0*M_PI/180.0)

#define RESIZE(_W_,_H_) \
    glViewport(0, 0, _W_, _H_); \
    m->win_w = _W_; m->win_h = _H_; \
    mat4_perspective(m->Projection, FE_DEFAULT_FOV, _W_/(float)_H_, \
                     FE_DEFAULT_NEAR, FE_DEFAULT_FAR); \
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

    m->fps_ceil = FE_DEFAULT_FPS_CEIL;
    m->framerate_limit = 0;

    if(m->framerate_limit <= 0)
		if(SDL_GL_SetSwapInterval(1) < 0)
			fe_logw(TAG, "Warning : Vsync is disabled. The FPS may skyrocket.\n");

    m->frameno = 0;
    m->lim_last_time = SDL_GetTicks();
    m->last_time = SDL_GetTicks();
}


void cube_main_clean_then_exit(void *arg) {
    struct cube_main *m = arg;

    glDeleteProgram(m->cube.prog);
    Cube_free(&m->cube);
    fe_globalstate_deinit(fe_gs);
    SDL_GL_DeleteContext(m->glctx);
    SDL_DestroyWindow(m->window);
    SDL_Quit();
    exit(EXIT_SUCCESS);
}

void (*cube_main_framefuncptr)(void *arg);

void cube_main_loop_iteration(void *arg) {
    struct cube_main *m = arg;

    uint32_t current_time;
    const double fps_counter_interval = 1000.0; /* Should be in [100, 1000] */
    bool dirty = false;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
            case SDL_APP_TERMINATING: 
                cube_main_framefuncptr = cube_main_clean_then_exit; 
                break;
            case SDL_APP_LOWMEMORY: break;
            case SDL_APP_WILLENTERBACKGROUND: break;
            case SDL_APP_DIDENTERBACKGROUND: break;
            case SDL_APP_WILLENTERFOREGROUND: break;
            case SDL_APP_DIDENTERFOREGROUND: break;
            case SDL_WINDOWEVENT: 
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    cube_main_framefuncptr = cube_main_clean_then_exit;
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
                            fe_logi(TAG, "Now using %s mode.\n", "fullscreen");
                        }
                        if(SDL_SetWindowFullscreen(m->window, 
                                (!is_fullscreen)*SDL_WINDOW_FULLSCREEN)<0) {
                            fe_logi(TAG, "Failed to toggle fullscreen : %s\n", SDL_GetError());
                            break;
                        }
                        if(is_fullscreen) {
                            SDL_SetWindowSize(m->window, m->old_win_w, m->old_win_h);
                            SDL_SetWindowPosition(m->window, 
                                    SDL_WINDOWPOS_CENTERED, 
                                    SDL_WINDOWPOS_CENTERED);
                            fe_logi(TAG, "Now using %s mode.\n", "windowed");
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
        fe_logi(TAG, "%u frames under %lg milliseconds = "
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
                m->framerate_limit = FE_FALLBACK_REFRESH_RATE;
                str = "fallback";
            }
            fe_logi(TAG, "Abnormal FPS detected (Vsync is not working). "
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
    glUniform1i(m->ufInvertLoc, 0);
    Cube_draw(&m->cube);

    glScissor(m->splitx*(float)m->win_w, 0, m->win_w-m->splitx*(float)m->win_w, m->win_h);
    glClearColor(0.7f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1i(m->ufInvertLoc, 1);
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


#ifdef FE_TARGET_WINDOWS
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   PSTR pCmdLine, int nCmdShow)
#elif defined FE_TARGET_ANDROID
int cube_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    struct cube_main m;
    cube_main_init(&m);
    cube_main_framefuncptr = cube_main_loop_iteration;
#ifdef FE_TARGET_EMSCRIPTEN
    emscripten_set_main_loop_arg(cube_main_framefuncptr, &m, 0, true);
#else
    for(;;)
        cube_main_framefuncptr(&m);
#endif
    return EXIT_SUCCESS;
}

