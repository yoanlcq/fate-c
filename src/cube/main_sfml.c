#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif
#include <fate.h>
#if defined(FATE_DEFS_WINDOWS)
#include <Windows.h>
#endif
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <linmath/linmath.h>
#include "cube.h"

#if defined(FATE_DEFS_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   PSTR pCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
    fate_globalstate_init(fate_gs);

    char *game_path = fate_sys_getgamepath();
    if(!game_path)
        exit(EXIT_FAILURE);
    fate_sys_set_current_directory(game_path);
    free(game_path);

    sfContext *ctx = sfContext_create();
    sfContextSettings ctxs = {24, 8, 0, 4, 3, 
        sfContextCore
#ifdef FATE_GL_DEBUG
            | sfContextDebug
#endif
    };

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

    size_t vmc = 1;
    const sfVideoMode *vms = sfVideoMode_getFullscreenModes(&vmc);
    sfVideoMode vm = (vmc > 0 ? vms[0] : sfVideoMode_getDesktopMode());
    vm.width  = 320;
    vm.height = 240;
    const uint16_t *vmr = fate_dpyres_prev(vm.width, vm.height);
    /*
    vm.width  = vmr[0];
    vm.height = vmr[1];
    */
    sfWindow *window = sfWindow_create(vm, "Early F.A.T.E cube demo",
            sfDefaultStyle, &ctxs);
    sfVector2i vec2i = {0, 0};
    sfWindow_setPosition(window, vec2i);
    sfWindow_setVerticalSyncEnabled(window, true);
    //sfWindow_setFramerateLimit(window, 60);
    sfWindow_setKeyRepeatEnabled(window, false);

    ctxs = sfWindow_getSettings(window);
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);

    GLint ctxflags, ctxpflags;
    GLboolean double_buffer, stereo_buffers;
    glGetIntegerv(GL_CONTEXT_FLAGS, &ctxflags);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &ctxpflags);
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
        "    Stencil buffer bits : %d\n"
        "    Antialiasing level  : %d%s\n",
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
        ctxs.depthBits,
        ctxs.stencilBits,
        ctxs.antialiasingLevel, ctxs.antialiasingLevel ? "x" : ""
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

    /* Must imperatively follow SFML Window creation. */
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


    sfVector2u winsiz;
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
    winsiz.x = _W_; winsiz.y = _H_; \
    mat4x4_perspective(Projection, 45, _W_/(float)_H_, 1.0f, 100.0f); \
    UPDATE_MVP();

    UPDATE_VIEW();
    RESIZE(vm.width, vm.height);

    int mousex=0, mousey=0;
    bool mousein = false, mousedown = false;
    float h_angle = 0.0f, v_angle = 0.0f;
    float L_x = 0.0f;
    float L_y = 0.0f;
    float R_x = 0.0f;
    float R_y = 0.0f;
    bool zoom_in  = false;
    bool zoom_out = false;
    double frameno = 0;
    sfClock *clock = sfClock_create();
    sfInt64 current_time, last_time = sfClock_restart(clock).microseconds;

    bool go_west = false, go_east = false;
    float splitx = 0.5f;
    bool running = true;
    bool dirty = false;

    while(running) {

        /* See http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/ */
        current_time = sfClock_getElapsedTime(clock).microseconds;
        ++frameno;
        if(current_time - last_time >= 100000LL)
        {
            /* fate_logf("%lf milliseconds/frame\n", 100.0/frameno); */
            frameno = 0;
            last_time += 100000LL;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);

        glScissor(0, 0, splitx*(float)winsiz.x, winsiz.y);
        glClearColor(0.3f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1ui(ufInvertLoc, 0);
        Cube_draw(&cube);

        glScissor(splitx*(float)winsiz.x, 0, winsiz.x-splitx*(float)winsiz.x, winsiz.y);
        glClearColor(0.7f, 0.1f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1ui(ufInvertLoc, 1);
        Cube_draw(&cube);
        
        glDisable(GL_SCISSOR_TEST); 

        sfWindow_display(window);

        sfEvent event;
        char *axis_str;
        while(sfWindow_pollEvent(window, &event)) {
            switch(event.type) {
                case sfEvtClosed:
                    running = false;
                    break;
               
                case sfEvtResized:
                    RESIZE(event.size.width, event.size.height);
                    break;
                case sfEvtLostFocus: break;
                case sfEvtGainedFocus: break;
                case sfEvtTextEntered: 
                    switch(event.text.unicode) {
                        case '<': case '>':
                            /* Ternary+function pointer combo. */
                            vmr = (event.text.unicode == '<' 
                                    ? fate_dpyres_prev_in_list
                                    : fate_dpyres_next_in_list)
                                    (vmr[0], vmr[1]);
                            winsiz.x = vmr[0];
                            winsiz.y = vmr[1];
                            sfWindow_setSize(window, winsiz);
                            break;
                    }
                    break;
                case sfEvtKeyPressed:
                    switch(event.key.code) {
                        case sfKeyZ:        L_y = -100.0f; break;
                        case sfKeyQ:        L_x = -100.0f; break;
                        case sfKeyS:        L_y =  100.0f; break;
                        case sfKeyD:        L_x =  100.0f; break;
                        case sfKeyUp:       R_y = -100.0f; break;
                        case sfKeyLeft:     R_x = -100.0f; go_west = true; break;
                        case sfKeyDown:     R_y =  100.0f; break;
                        case sfKeyRight:    R_x =  100.0f; go_east = true; break;
                        case sfKeyAdd:      zoom_in  = true; break;
                        case sfKeySubtract: zoom_out = true; break;
                    }
                    break;
                case sfEvtKeyReleased: 
                    switch(event.key.code) {
                        case sfKeyZ:        if(L_y < 0.0f) L_y = 0.0f; break;
                        case sfKeyQ:        if(L_x < 0.0f) L_x = 0.0f; break;
                        case sfKeyS:        if(L_y > 0.0f) L_y = 0.0f; break;
                        case sfKeyD:        if(L_x > 0.0f) L_x = 0.0f; break;
                        case sfKeyUp:       if(R_y < 0.0f) R_y = 0.0f; break;
                        case sfKeyLeft:     if(R_x < 0.0f) R_x = 0.0f; go_west = false; break;
                        case sfKeyDown:     if(R_y > 0.0f) R_y = 0.0f; break;
                        case sfKeyRight:    if(R_x > 0.0f) R_x = 0.0f; go_east = false; break;
                        case sfKeyAdd:      zoom_in  = false; break;
                        case sfKeySubtract: zoom_out = false; break;
                        case sfKeyF11:
    /*fullscreen_motif_wm_hints(sfWindow_getSystemHandle(window)); */
                            break;

                    }
                    break;
                case sfEvtMouseWheelScrolled: 
                    distance -= event.mouseWheelScroll.delta;        
                    dirty = true;
                    break;
                case sfEvtMouseButtonPressed:  mousedown = true;  break;
                case sfEvtMouseButtonReleased: mousedown = false; break;
                case sfEvtMouseMoved: 

                    if(!mousein) {
                        mousex = event.mouseMove.x;
                        mousey = event.mouseMove.y;
                        mousein = true;
                    }
                    if(mousedown) {
                        h_angle += (event.mouseMove.x - mousex)*M_PI/180.0f;
                        v_angle += (event.mouseMove.y - mousey)*M_PI/180.0f;
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
                    mousex = event.mouseMove.x;
                    mousey = event.mouseMove.y;

                    break;
                case sfEvtMouseEntered:
                    mousein = true;
                    break;
                case sfEvtMouseLeft: 
                    mousein = false;
                    break;
                case sfEvtJoystickButtonPressed:
                case sfEvtJoystickButtonReleased:
                    break;
                case sfEvtJoystickMoved:
                    switch(event.joystickMove.axis) {
                        case sfJoystickX: 
                            L_x = event.joystickMove.position;
                            axis_str = "X";
                            break;
                        case sfJoystickY: 
                            L_y = event.joystickMove.position;
                            axis_str = "Y"; 
                            break;
                        case sfJoystickU: 
                            R_x = event.joystickMove.position;
                            axis_str = "U";
                            break;
                        case sfJoystickR: 
                            R_y = event.joystickMove.position;
                            axis_str = "R"; 
                            break;
                        case sfJoystickZ:   axis_str = "Z";    break;
                        case sfJoystickV:   axis_str = "V";    break;
                        case sfJoystickPovX: axis_str = "PovX"; break;
                        case sfJoystickPovY: 
                            if(event.joystickMove.position == 0.0f)
                                zoom_in = zoom_out = false;
                            else if(event.joystickMove.position < 0.0f)
                                zoom_in = true, zoom_out = false;
                            else if(event.joystickMove.position > 0.0f)
                                zoom_in = false, zoom_out = true;
                            axis_str = "PovY";
                            break;
                    }
                    printf("Joystick %d: %s axis at position %f\n", 
                            event.joystickMove.joystickId, 
                            axis_str, 
                            event.joystickMove.position);
                    break;
                case sfEvtJoystickConnected: 
                case sfEvtJoystickDisconnected: 
                    printf("Joystick %d: %sconnected.\n", 
                            event.joystickConnect.joystickId, 
                            event.type==sfEvtJoystickConnected ? "" : "dis");
                    break;
                case sfEvtTouchBegan: break;
                case sfEvtTouchMoved: break;
                case sfEvtTouchEnded: break;
                case sfEvtSensorChanged: break;
                case sfEvtCount: 
                    //Useless. 
                    break;
                case sfEvtMouseWheelMoved: 
                    //Deprecated.  
                    break;
            } /* end switch(event.type) */
        } /* end while(sfWindow_pollEvent(window) */

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

        /* TODO traiter évènements */

    } /* end while(running) */
    Cube_free(&cube);
    glDeleteProgram(progid);
    sfWindow_destroy(window); /* Also performs close(), which deletes the OpenGL context. */
    sfContext_destroy(ctx);
    sfClock_destroy(clock);
    fate_globalstate_deinit(fate_gs);
    exit(EXIT_SUCCESS);
}
