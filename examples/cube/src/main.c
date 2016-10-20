#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_revision.h>
#include <linmath/linmath.h>
#ifdef FE_TARGET_EMSCRIPTEN
#include <emscripten.h>
#endif
#include "cube.h"
#include "bgmus.h"

#define FE_GAME_NAME "Rainbow Dice"
#define FE_GAME_ID "Rainbow_Dice"
fe_pragma_message("Testing the fe_pragma_message().")

struct cube_main {
    SDL_Window *window;
    SDL_DisplayMode current_display_mode;
    SDL_GLContext glctx; 
    uint16_t win_w, win_h;
    uint16_t old_win_w, old_win_h;
    Cube cube, skybox, gizmo;
    float distance;
    vec3 eye, center, up;
    mat4 Projection, View, Model;
    int mousex, mousey;
    bool mousein, mousedown;
    SDL_FingerID finger_id;
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
    SDL_SetHintWithPriority(SDL_HINT_NO_SIGNAL_HANDLERS, "1", 
                            SDL_HINT_OVERRIDE);
    SDL_SetHintWithPriority(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1", SDL_HINT_OVERRIDE);
#else
    /* XXX We should issue a compile-time error here, but I'm not doing
     * it because it would break Travis builds (and some older Ubuntu
     * versions) */
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
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); //??
    /*
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 
                        0 //SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
#if defined(FE_GL_DBG) && defined(FE_GL_TARGET_DESKTOP)
                       | SDL_GL_CONTEXT_DEBUG_FLAG
#endif
                        );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, 
#ifdef FE_GL_TARGET_DESKTOP
                        SDL_GL_CONTEXT_PROFILE_CORE
#else
                        SDL_GL_CONTEXT_PROFILE_ES
#endif
    );
#ifdef FE_GL_TARGET_ES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
#endif
/*
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

    /* Seems redundant, but actually creating an SDL2 window
     * does not imply GL context creation.
     * SDL2's docs states that you need to call SDL_GL_CreateContext() before
     * any GL calls. */
    m->glctx = SDL_GL_CreateContext(m->window);
    if(!m->glctx)
        fe_fatal(TAG, "SDL_GL_CreateContext failed : %s\n", SDL_GetError());

#ifdef FE_GL_USE_GLAD
    /* It returns false on my laptop, even though functions are properly loaded.
     * Seen it though GDB. */
    #ifdef FE_GL_TARGET_DESKTOP
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    #else
    gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    #endif
#elif defined(FE_GL_USE_GLEW)
    glewExperimental = GL_TRUE;
    fe_logi(TAG, "Using GLEW %s\n", glewGetString(GLEW_VERSION));
    GLenum glew = glewInit();
    if(glew != GLEW_OK)
        fe_fatal(TAG, "Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glew));
#endif
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
#ifdef FE_GL_TARGET_DESKTOP
            3, 0, 
#elif defined(FE_TARGET_EMSCRIPTEN)
            1, 0, 
#else
            2, 0, 
#endif
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
        es = gl_version.es;
        fe_logi(TAG, "Parsed OpenGL version : %u.%u%s\n", 
                     maj, min, es ? " ES" : "");
        bool supported = (es ? maj>=2 : maj>=3);
        if(!supported)
            fe_fatal(TAG, 
                    "The OpenGL version reported by your driver is not "
                    "supported because it is too low.\n"
                    "Please let me know about it so I see how many people"
                    "require support for it.\n");
    }
    GLint ctxflags, ctxpflags, depth_bits, stencil_bits;
    GLboolean double_buffer, stereo_buffers;

#ifndef FE_TARGET_EMSCRIPTEN
    glGetIntegerv(GL_CONTEXT_FLAGS, &ctxflags);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH, 
            GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_bits);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, 
            GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);
    glGetBooleanv(GL_DOUBLEBUFFER, &double_buffer);
    glGetBooleanv(GL_STEREO, &stereo_buffers);
#else
    ctxflags=0, ctxpflags=0, depth_bits=24, stencil_bits=8;
    double_buffer=true, stereo_buffers=false;
#endif
#ifdef FE_GL_TARGET_ES
    ctxpflags = 0;
#endif


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

    size_t i;
    fe_logi(TAG, "    Extensions :\n");
#ifdef FE_GL_TARGET_DESKTOP
    GLint num_glexts;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_glexts);
    for(i=0 ; i<num_glexts ; i++)
        fe_logi(TAG, "%s\n", glGetStringi(GL_EXTENSIONS, i));
#else
    const char *cur = (const char*) glGetString(GL_EXTENSIONS);
    for(;;) {
        const char *spc = strchr(cur, ' ');
        if(!spc)
            spc = strchr(cur, '\0');
        fe_logi(TAG, "%.*s\n", (int)(spc-cur), cur);
        if(!*spc)
            break;
        cur = spc+1;
    }
#endif

    {
        GLint num_glfmts;
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_glfmts);
        GLint *fmts = fe_mem_heapalloc(num_glfmts, GLint, "");
        fe_dbg_hope(fmts);
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, fmts);
        fe_logi(TAG, "\n    Compressed texture formats :\n\n");
        for(i=0 ; i<num_glfmts ; i++)
            fe_logi(TAG, "0x%.4"PRIx32": %s\n", (int32_t)fmts[i],
                    fe_gl_tc_format_to_name(fmts[i]));
        fe_mem_heapfree(fmts);
    }

    {
        fe_logi(TAG, "\n    Limits :\n\n");
        GLint val;
#define HELPER(CST, req) \
        glGetIntegerv(GL_MAX_##CST, &val); \
        fe_logi(TAG, "GL_MAX_%-28s : %d (standard: %d)\n", #CST, (int)val, req)
        HELPER(RENDERBUFFER_SIZE           ,   1);
        HELPER(TEXTURE_IMAGE_UNITS         ,   8);
        HELPER(COMBINED_TEXTURE_IMAGE_UNITS,   8);
        HELPER(TEXTURE_SIZE                ,  64);
        HELPER(CUBE_MAP_TEXTURE_SIZE       ,  16);
        HELPER(VERTEX_ATTRIBS              ,   8);
        HELPER(VERTEX_TEXTURE_IMAGE_UNITS  ,   0);
        HELPER(VERTEX_UNIFORM_VECTORS      , 128);
        HELPER(VARYING_VECTORS             ,   8);
        HELPER(FRAGMENT_UNIFORM_VECTORS    ,  16);
#undef HELPER
        GLint dims[2];
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);
        fe_logi(TAG, "GL_MAX_%-28s : %dx%d\n", "VIEWPORT_DIMS", 
                (int)dims[0], (int)dims[1]);
    }


    fe_gl_dbg_setup(&gl_version, true);
    fe_gl_mkprog_setup(&gl_version);
    Cube_setup();
    fe_gl_mkprog_cleanup();
    Cube_init(&m->cube);
    Cube_init(&m->skybox);
    Cube_init(&m->gizmo);

    m->skybox.cullface = CUBE_FACE_FRONT;
    m->skybox.material_for_back = 0;
    m->skybox.ambient[0] = m->skybox.ambient[1] = 1.0f;
    m->skybox.ambient[2] = m->skybox.ambient[3] = 1.0f;
    m->cube.cullface = CUBE_FACE_NONE;
    m->cube.material_for_front = 1;
    m->cube.material_for_back = 0;
    m->cube.ambient[0] = 0.3f;
    m->cube.ambient[1] = 0.5f;
    m->cube.ambient[2] = 1.0f;
    m->cube.ambient[3] = 1.0f;
    m->gizmo.cullface = CUBE_FACE_BACK;
    m->gizmo.material_for_front = 1;
    m->gizmo.ambient[0] = m->gizmo.ambient[1] = 1.0f;
    m->gizmo.ambient[2] = m->gizmo.ambient[3] = 1.0f;


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    m->distance = 10.0f;
    m->eye[0] = 0.0f;
    m->eye[1] = 0.0f;
    m->eye[2] = m->distance;
    memset(m->center, 0, sizeof(vec3));
    m->up[0] = 0.0f;
    m->up[1] = 1.0f;
    m->up[2] = 0.0f;

#define FE_FALLBACK_REFRESH_RATE 60
#define FE_DEFAULT_FPS_CEIL 256
#define FE_DEFAULT_NEAR 0.25
#define FE_DEFAULT_FAR  1024.0
#define FE_DEFAULT_FOV  (75.0*FE_PI_F/180.0)

#define UPDATE_VIEW() \
    mat4_look_at(m->View, m->eye, m->center, m->up)

#define UPDATE_MVP() \
    { \
        mat4 fake_view; \
        vec3 fake_eye; \
        vec3_norm(fake_eye, m->eye); \
        vec3_scale(fake_eye, fake_eye, 14.f); \
        mat4_look_at(fake_view, fake_eye, m->center, m->up); \
        mat4_identity(m->gizmo.mvp_matrix); \
        mat4_translate(m->gizmo.mvp_matrix, -.8f, -.8f, 0.f); \
        memcpy(m->gizmo.mv_matrix, m->gizmo.mvp_matrix, sizeof(mat4)); \
        mat4_identity(m->Model); \
        mat4_mul(m->gizmo.mvp_matrix, m->gizmo.mvp_matrix, m->Projection); \
        mat4_mul(m->gizmo.mvp_matrix, m->gizmo.mvp_matrix, fake_view); \
        mat4_mul(m->gizmo.mvp_matrix, m->gizmo.mvp_matrix, m->Model); \
        mat4_mul(m->gizmo.mv_matrix, m->gizmo.mv_matrix, fake_view); \
        mat4_mul(m->gizmo.mv_matrix, m->gizmo.mv_matrix, m->Model); \
        mat4 tmp_inv_matrix; \
        mat4_invert(tmp_inv_matrix, m->gizmo.mv_matrix); \
        mat4_transpose(m->gizmo.normal_matrix, tmp_inv_matrix); \
        mat4_identity(m->gizmo.normal_matrix); /* XXX HACK */\
    } {\
        mat4_identity(m->Model); \
        mat4_translate(m->Model, m->eye[0], m->eye[1], m->eye[2]); \
        mat4_scale_aniso(m->Model, m->Model, FE_DEFAULT_FAR/2.f, FE_DEFAULT_FAR/2.f, FE_DEFAULT_FAR/2.f); \
        mat4_mul(m->skybox.mvp_matrix, m->Projection, m->View); \
        mat4_mul(m->skybox.mvp_matrix, m->skybox.mvp_matrix, m->Model); \
        mat4 tmp_inv_matrix; \
        mat4_invert(tmp_inv_matrix, m->skybox.mv_matrix); \
        mat4_transpose(m->skybox.normal_matrix, tmp_inv_matrix); \
        mat4_identity(m->skybox.normal_matrix); /* XXX HACK */\
    } {\
        mat4_identity(m->Model); \
        mat4_mul(m->cube.mvp_matrix, m->Projection, m->View); \
        mat4_mul(m->cube.mvp_matrix, m->cube.mvp_matrix, m->Model); \
        mat4_mul(m->cube.mv_matrix, m->View, m->Model); \
        mat4 tmp_inv_matrix; \
        mat4_invert(tmp_inv_matrix, m->cube.mv_matrix); \
        mat4_transpose(m->cube.normal_matrix, tmp_inv_matrix); \
        mat4_identity(m->cube.normal_matrix); /* XXX HACK */\
    }


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
#if defined(FE_TARGET_ANDROID) || defined(FE_TARGET_IOS)
    m->mousein = true;
#endif
    m->h_angle = m->v_angle = 0.0f;
    m->R_x = m->R_y = 0.0f;
    m->zoom_in = m->zoom_out = false;

    m->go_west = m->go_east = false;
    m->splitx = 0.5f;

    m->fps_ceil = FE_DEFAULT_FPS_CEIL;
    m->framerate_limit = 0;

#ifndef FE_TARGET_EMSCRIPTEN
    if(m->framerate_limit <= 0)
		if(SDL_GL_SetSwapInterval(1) < 0)
			fe_logw(TAG, "Warning : Vsync is disabled. The FPS may skyrocket.\n");
#endif

    m->frameno = 0;
    m->lim_last_time = SDL_GetTicks();
    m->last_time = SDL_GetTicks();

    bgmus_setup();
    bgmus_play();
}


void cube_main_clean_then_exit(void *arg) {
    struct cube_main *m = arg;

    Cube_deinit(&m->cube);
    Cube_deinit(&m->skybox);
    Cube_deinit(&m->gizmo);
    Cube_cleanup();
    bgmus_cleanup();
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
            case SDL_FINGERMOTION: 
                if(event.tfinger.fingerId != m->finger_id)
                    break;
                m->h_angle -= 4.2f*event.tfinger.dx;
                m->v_angle += 4.2f*event.tfinger.dy;

                    if(m->v_angle >= FE_PI_F/2.f)
                        m->v_angle = FE_PI_F/2.f - FE_PI_F/180.0f;
                    else if(m->v_angle <= -FE_PI_F/2.f)
                        m->v_angle = -FE_PI_F/2.f + FE_PI_F/180.0f;
                    m->eye[0] =  m->distance*sinf(m->h_angle)*cosf(m->v_angle);
                    m->eye[1] =  m->distance*sinf(m->v_angle);
                    m->eye[2] =  m->distance*cosf(m->h_angle)*cosf(m->v_angle);
                    UPDATE_VIEW();
                    UPDATE_MVP();

                break;
            case SDL_MOUSEMOTION: 
                
                if(!m->mousein) {
                    m->mousex = event.motion.x;
                    m->mousey = event.motion.y;
                    m->mousein = true;
                }
                if(m->mousedown) {
                    m->h_angle -= (event.motion.x - m->mousex)*FE_PI_F/180.0f;
                    m->v_angle += (event.motion.y - m->mousey)*FE_PI_F/180.0f;
                    if(m->v_angle >= FE_PI_F/2.f)
                        m->v_angle = FE_PI_F/2.f - FE_PI_F/180.0f;
                    else if(m->v_angle <= -FE_PI_F/2.f)
                        m->v_angle = -FE_PI_F/2.f + FE_PI_F/180.0f;
                    m->eye[0] =  m->distance*sinf(m->h_angle)*cosf(m->v_angle);
                    m->eye[1] =  m->distance*sinf(m->v_angle);
                    m->eye[2] =  m->distance*cosf(m->h_angle)*cosf(m->v_angle);

                    UPDATE_VIEW();
                    UPDATE_MVP();
                }    
                m->mousex = event.motion.x;
                m->mousey = event.motion.y;
                break;
            case SDL_FINGERDOWN: 
                if(m->mousedown)
                    break;
                m->finger_id = event.tfinger.fingerId;
            case SDL_MOUSEBUTTONDOWN: 
                m->mousedown = true;                  
                break;
            case SDL_FINGERUP: 
                if(event.tfinger.fingerId != m->finger_id) 
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
            case SDL_DOLLARGESTURE: break;
            case SDL_DOLLARRECORD: break;
            case SDL_MULTIGESTURE: 
                m->mousedown = false;
                m->distance -= 42.f*event.mgesture.dDist;
                dirty = true;
                break;
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
        vec3 old_eye = {m->eye[0], m->eye[1], m->eye[2]};
        m->eye[0] =  m->distance*sinf(m->h_angle)*cosf(m->v_angle);
        m->eye[1] =  m->distance*sinf(m->v_angle);
        m->eye[2] =  m->distance*cosf(m->h_angle)*cosf(m->v_angle);
        /* Hack to prevent having a clear-color fullscreen. */
        if(m->eye[0]==0.f && m->eye[1]==0.f && m->eye[2]==0.f) {
            m->eye[0] = old_eye[0]*0.0001f;
            m->eye[1] = old_eye[1]*0.0001f;
            m->eye[2] = old_eye[2]*0.0001f;
        }
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

    glClearColor(0.3f, 0.7f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 eye_direction;
    vec3_sub(eye_direction, m->eye, m->center);
    const Cube * cubes[2] = {&m->cube, &m->skybox};
    Cube_multidraw(cubes, 2, eye_direction);
    const Cube *cube_ptr = &m->gizmo;
    glDisable(GL_DEPTH_TEST);
    Cube_multidraw(&cube_ptr, 1, eye_direction);
    glEnable(GL_DEPTH_TEST);

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
    #ifdef _MSC_VER
    int wmain(int argc, wchar_t* argv[])
    #else
    /* Don't try wWinMain() : Doesn't build on MinGW. */
    int WINAPI WinMain(
        HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        int         nCmdShow
        )
    #endif
#elif defined FE_TARGET_ANDROID
int cube_demo_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    struct cube_main m;
    cube_main_init(&m);
    cube_main_framefuncptr = cube_main_loop_iteration;
#ifdef FE_TARGET_EMSCRIPTEN
    emscripten_set_main_loop_arg(cube_main_framefuncptr, &m, -1, true);
#else
    for(;;)
        cube_main_framefuncptr(&m);
#endif
    return EXIT_SUCCESS;
}

