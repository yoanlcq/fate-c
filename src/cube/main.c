#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <linmath/linmath.h>
#include <fate.h>
#include "cube.h"

int main(int argc, char *argv[])
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

    GLint gl_major, gl_minor;
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);

    fate_logf_video(
            "--- OpenGL version ---\n"
            "\tRequired  : >= %d.%d\n"
            "\tSupported :    %s\n"
            "\n"
            "--- OpenGL device ---\n"
            "\tRenderer  : %s\n"
            "\tVendor    : %s\n"
            "\n",
            gl_major, gl_minor, 
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

    if(gl_major < 4 || (gl_major == 4 && gl_minor < 3))
    {
        fate_logf_err("The OpenGL version reported by your driver is not yet"
                      "supported.\nSorry. I'm working on it.\n");
        exit(EXIT_FAILURE);
    }

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if(glewInitResult != GLEW_OK)
    {
        fate_logf_err("Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }

    size_t vmc = 1;
    const sfVideoMode *vms = sfVideoMode_getFullscreenModes(&vmc);
    sfVideoMode vm = (vmc > 0 ? vms[0] : sfVideoMode_getDesktopMode());
    vm.width  = 640;
    vm.height = 480;
    const uint16_t *vmr = fate_dpyres_prev_in_list(vm.width, vm.height);
    vm.width  = vmr[0];
    vm.height = vmr[1];
    sfVector2u winsiz;
    winsiz.x = vm.width;
    winsiz.y = vm.height;
    sfWindow *window = sfWindow_create(vm, "Early F.A.T.E cube demo",
            sfDefaultStyle, &ctxs);
    sfVector2i vec2i = {0, 0};
    sfWindow_setPosition(window, vec2i);
    sfWindow_setFramerateLimit(window, 60);
    sfWindow_setKeyRepeatEnabled(window, false);

    ctxs = sfWindow_getSettings(window);

    fate_logf_video(
            "--- Active OpenGL context settings ---\n"
            "\tVersion             : %d.%d\n"
            "\tGLSL version        : %s\n"
            "\tDepth buffer bits   : %d\n"
            "\tStencil buffer bits : %d\n"
            "\tAntialiasing level  : %dx\n",
            ctxs.majorVersion, ctxs.minorVersion,
            glGetString(GL_SHADING_LANGUAGE_VERSION),
            ctxs.depthBits,
            ctxs.stencilBits,
            ctxs.antialiasingLevel);
    int num_glexts, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_glexts);
    fate_logf_video("\tExtensions :");
    for(i=0 ; i<num_glexts ; ++i) 
        fate_logf_video(" %s", glGetStringi(GL_EXTENSIONS, i));
    fate_logf_video("\n");

    /* Must imperatively follow SFML Window creation. */
    fate_gl_debug_setup(gl_major, gl_minor, true);
    fate_glDebugMessageCallback((DEBUGPROC) &fate_gl_debug_msg_callback, 
            NULL);
    fate_glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);

    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Kind of breaks transparency ? */
    /* glEnable(GL_CULL_FACE); */
    /* glCullFace(GL_FRONT); */

    fate_gl_mkprog_setup(gl_major, gl_minor);
    GLuint progid = glCreateProgram();
    if(!fate_gl_mkprog(progid,
                       "data/OpenGL/triangles.glb",
                       "res/shaders/triangles.vert",
                       "res/shaders/triangles.frag",
                       NULL))
    {
        fate_logf_err("Can't continue.\n");
        exit(EXIT_FAILURE);
    }
    fate_gl_mkprog_cleanup();
    glUseProgram(progid); 

    Cube cube;
    Cube_init(&cube);

    float distance = 6.0f;
    vec3 eye = {0.0f, 0.0f, -distance}, center = {0,0,0}, up = {0,1,0};
    GLint MVPMatrixLoc = glGetUniformLocation(progid, "MVPMatrix");
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
    mat4x4_perspective(Projection, 45, _W_/(float)_H_, 1.0f, 100.0f); \
    UPDATE_MVP();

    UPDATE_VIEW();
    RESIZE(vm.width, vm.height);

    int mousex, mousey;
    bool mousein = false, mousedown = false;
    float h_angle = 0.0f, v_angle = 0.0f;
    float L_x = 0.0f;
    float L_y = 0.0f;
    float R_x = 0.0f;
    float R_y = 0.0f;
    bool zoom_in  = false;
    bool zoom_out = false;
    bool fullscreen = false;
    double frameno = 0;
    sfClock *clock = sfClock_create();
    sfInt64 current_time, last_time = sfClock_restart(clock).microseconds;

    bool running = true;
    /* glClearColor(0.3f, 0.9f, 1.0f, 1.0f); */
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Cube_draw(&cube);

        sfWindow_display(window);

        sfEvent event;
        sfVector2u winsiz;
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
                        case sfKeyZ:        distance += 1.0f; L_y = -100.0f; break;
                        case sfKeyQ:        distance -= 1.0f; L_x = -100.0f; break;
                        case sfKeyS:        L_y =  100.0f; break;
                        case sfKeyD:        L_x =  100.0f; break;
                        case sfKeyUp:       R_y = -100.0f; break;
                        case sfKeyLeft:     R_x = -100.0f; break;
                        case sfKeyDown:     R_y =  100.0f; break;
                        case sfKeyRight:    R_x =  100.0f; break;
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
                        case sfKeyLeft:     if(R_x < 0.0f) R_x = 0.0f; break;
                        case sfKeyDown:     if(R_y > 0.0f) R_y = 0.0f; break;
                        case sfKeyRight:    if(R_x > 0.0f) R_x = 0.0f; break;
                        case sfKeyAdd:      zoom_in  = false; break;
                        case sfKeySubtract: zoom_out = false; break;
                        case sfKeyF11: break;
                    }
                    break;
                case sfEvtMouseWheelScrolled: break;
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
        
        if(R_x > 10.0f || R_x < -10.0f || R_y>10.0f || R_y<-10.0f) {
            eye[0] += R_x/200.0f;
            eye[1] += R_y/200.0f;
            UPDATE_VIEW();
            UPDATE_MVP();
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
