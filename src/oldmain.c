#include <opengl_base.h>
#include <opengl_debug.h>

#if CSFML_VERSION_MAJOR != 2 || CSFML_VERSION_MINOR != 3
#error We target CSFML 2.3. Not above, not under.
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <linmath/linmath.h>
#include <utils/pathto.h>
#include <utils/display_resolutions.h>
#include <glmake.h>
#include <cube.h>

#ifdef _WIN32
#include <windows.h>
#include <Wingdi.h>
HGLRC saved_opengl_context;
BOOL saved_opengl_context_is_initialized = FALSE;

/*
 *Basically it's just resizing the window and specifying flags that the border is invisible.

 SetWindowLongPtr(hWnd, GWL_STYLE, 
     WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
     MoveWindow(hWnd, 0, 0, width, height, TRUE);
     
     to set it back:

     RECT rect;
     rect.left = 0;
     rect.top = 0;
     rect.right = width;
     rect.bottom = height;
     SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
     AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
     MoveWindow(hWnd, 0, 0, rect.right-rect.left, rect.bottom-rect.top, TRUE);
     
     or for a not-resizable window:

     SetWindowLongPtr(hWnd, GWL_STYLE, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE);
     AdjustWindowRect(&rect, WS_CAPTION | WS_POPUPWINDOW, FALSE);
     MoveWindow(hWnd, 0, 0, rect.right-rect.left, rect.bottom-rect.top, TRUE);

     and then just resize your OpenGL viewport settings.

     If you want to set the display mode too, use this:

     change display mode if destination mode is fullscreen
     if (fullscreen) {
         DEVMODE dm;
         dm.dmSize = sizeof(DEVMODE);
         dm.dmPelsWidth = width;
         dm.dmPelsHeight = height;
         dm.dmBitsPerPel = bitsPerPixel;
         dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
         success = ChangeDisplaySettings(&dm, 0) == DISP_CHANGE_SUCCESSFUL;
     }
     // reset display mode if destination mode is windowed
     if (!fullscreen)
         success = ChangeDisplaySettings(0, 0) == DISP_CHANGE_SUCCESSFUL;
*
* */
void save_opengl_context(sfWindowHandle handle) 
{
if(!saved_opengl_context_is_initialized) {
    saved_opengl_context = wglCreateContext(wglGetCurrentDC());
    saved_opengl_context_is_initialized = TRUE;
}
    wglCopyContext(wglGetCurrentContext(),
                   saved_opengl_context,
                   GL_ALL_ATTRIB_BITS);
}

void restore_opengl_context(sfWindowHandle handle)
{
    wglMakeCurrent(wglGetCurrentDC(), saved_opengl_context);
/*
    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if(glewInitResult != GLEW_OK)
    {
        fprintf(stderr, "Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }
#ifdef OPENGL_DEBUG
    glDebugMessageCallback((GLDEBUGPROC) &opengl_debug_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
#endif
*/
}

#elif defined(__linux__)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
GLXContext saved_opengl_context;
bool saved_opengl_context_is_initialized = false;
/*
One way is to bypass the window manager:
http://stackoverflow.com/questions/9065669/x11-glx-fullscreen-mode
XSetWindowAttributes wa;                                                     
wa.override_redirect = True;                                           
XCreateWindow( ..., &wa );
*/
void save_opengl_context(sfWindowHandle handle)
{
    /*
    if(!saved_opengl_context_is_initialized) {
        saved_opengl_context = glXCreateContext(handle, vis, NULL, true);
        saved_opengl_context_is_initialized = true;
    }
    glXCopyContext(handle,
                   wglGetCurrentContext(),
                   saved_opengl_context,
                   GL_ALL_ATTRIB_BITS);
                   */
}

void restore_opengl_context(sfWindowHandle handle)
{
}
#endif




int main(int argc, char *argv[])
{
    sfContext *ctx = sfContext_create();
    sfContextSettings ctxs = {24, 8, 0, 4, 3, 
        sfContextCore
#ifdef OPENGL_DEBUG
            |sfContextDebug
#endif
    };

    fprintf(stdout, 
            "--- OpenGL version ---\n"
            "    Required  : >= %d.%d\n"
            "    Supported :    %s\n"
            "\n"
            "--- OpenGL device ---\n"
            "    Renderer  : %s\n"
            "    Vendor    : %s\n"
            "\n",
            ctxs.majorVersion, ctxs.minorVersion, 
            glGetString(GL_VERSION),
            glGetString(GL_RENDERER),
            glGetString(GL_VENDOR));

    GLint gl_version[2];
    glGetIntegerv(GL_MAJOR_VERSION, &gl_version[0]);
    glGetIntegerv(GL_MINOR_VERSION, &gl_version[1]);

    if(gl_version[0] < 4 && gl_version[1] < 3)
    {
        fprintf(stderr, 
                "Your hardware does not meet the requirements.\n"
                "Make sure that the device used is the one you are expecting, "
                "and that your drivers are up-to-date.\n");
        exit(EXIT_FAILURE);
    }

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if(glewInitResult != GLEW_OK)
    {
        fprintf(stderr, "Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }

    size_t vmc = 1;
    const sfVideoMode *vms = sfVideoMode_getFullscreenModes(&vmc);
    sfVideoMode vm = (vmc > 0 ? vms[0] : sfVideoMode_getDesktopMode());
    vm.width  = 640;
    vm.height = 480;
    const uint16_t *vmr = prev_resolution(vm.width, vm.height);
    vm.width  = vmr[0];
    vm.height = vmr[1];
    sfVector2u winsiz;
    winsiz.x = vm.width;
    winsiz.y = vm.height;
    sfWindow *window = sfWindow_create(vm, "OpenGL", sfDefaultStyle, &ctxs);
    sfVector2i vec2i = {0, 0};
    sfWindow_setPosition(window, vec2i);
    sfWindow_setFramerateLimit(window, 60);
    sfWindow_setKeyRepeatEnabled(window, false);

    ctxs = sfWindow_getSettings(window);

    fprintf(stdout,
            "--- Active OpenGL context settings ---\n"
            "    Version             : %d.%d\n"
            "    GLSL version        : %s\n"
            "    Depth buffer bits   : %d\n"
            "    Stencil buffer bits : %d\n"
            "    Antialiasing level  : %dx\n",
            ctxs.majorVersion, ctxs.minorVersion,
            glGetString(GL_SHADING_LANGUAGE_VERSION),
            ctxs.depthBits,
            ctxs.stencilBits,
            ctxs.antialiasingLevel);
    int num_glexts, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_glexts);
    printf("    Extensions :\n");
    for(i=0 ; i<num_glexts ; ++i) 
        printf("        %s\n", glGetStringi(GL_EXTENSIONS, i));
    putchar('\n');

#ifdef OPENGL_DEBUG
    /* Must imperatively follow SFML Window creation. */
    glDebugMessageCallback((GLDEBUGPROC) &opengl_debug_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
#endif

    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Kind of breaks transparency ? */
    /* glEnable(GL_CULL_FACE); */
    /* glCullFace(GL_FRONT); */

    GLuint progid = glCreateProgram();
    if(!glmake(progid,
               pathto("@/data/OpenGL/triangles.glb"),
               pathto("@/res/shaders/triangles.vert"),
               pathto("@/res/shaders/triangles.frag"),
               NULL))
    {
        fprintf(stderr, "Can't continue.\n");
        exit(EXIT_FAILURE);
    }
    glmake_clean();
    glUseProgram(progid); 

    Cube cube;
    Cube_init(&cube);

    float distance = 6.0f;

    vec3 eye = {0.0f, 0.0f, -distance}, center = {0,0,0}, up = {0,1,0};
    mat4x4 MVPMatrix, Projection, View, Model;
    mat4x4_identity(Model);
    mat4x4_look_at(View, eye, center, up);
    mat4x4_perspective(Projection, 45, vm.width/(float)vm.height, 1.0f, 100.0f);
    mat4x4_dup(MVPMatrix, Model);
    mat4x4_mul(MVPMatrix, View, MVPMatrix);
    mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
    GLint MVPMatrixLoc = glGetUniformLocation(progid, "MVPMatrix");
    glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);


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
            /* printf("%lf milliseconds/frame\n", 100.0/frameno); */
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
                    glViewport(0, 0, event.size.width, event.size.height);
                    mat4x4_perspective(Projection, 45, event.size.width/(float)event.size.height, 1.0f, 100.0f);
                    mat4x4_dup(MVPMatrix, Model);
                    mat4x4_mul(MVPMatrix, View, MVPMatrix);
                    mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
                    glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);
                    break;
                case sfEvtLostFocus: break;
                case sfEvtGainedFocus: break;
                case sfEvtTextEntered: 
                    switch(event.text.unicode) {
                        case '<': case '>':
                            /* Ternary+function pointer combo. */
                            vmr = (event.text.unicode == '<' 
                                    ? prev_resolution 
                                    : next_resolution)
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
                        case sfKeyF11: 
                            printf("\n%sing Fullscreen.\n\n", fullscreen ? "Leav" : "Go");
                            /* TODO The true way is a reinit_everything function. */
                            save_opengl_context(sfWindow_getSystemHandle(window));
                            sfWindow_destroy(window);
                            if(fullscreen) {
                                vm.width = winsiz.x;
                                vm.height= winsiz.y;
                                window = sfWindow_create(vm, "OpenGL", sfDefaultStyle, &ctxs);
                            } else {
                                vm = sfVideoMode_getFullscreenModes(NULL)[0];
                                winsiz.x = vm.width;
                                winsiz.y = vm.height;
                                window = sfWindow_create(vm, "OpenGL", sfFullscreen, &ctxs);
                            }






                            restore_opengl_context(sfWindow_getSystemHandle(window));

    ctxs = sfWindow_getSettings(window);

    glEnable(GL_PRIMITIVE_RESTART);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    progid = glCreateProgram();
    if(!glmake(progid,
               pathto("@/res/precomp/triangles.bin"),
               pathto("@/res/shaders/triangles.vert"),
               pathto("@/res/shaders/triangles.frag"),
               NULL))
    {
        fprintf(stderr, "Can't continue.\n");
        exit(EXIT_FAILURE);
    }
    glmake_clean();
    glUseProgram(progid); 

    Cube_free(&cube);
    Cube_init(&cube);

    mat4x4_identity(Model);
    mat4x4_look_at(View, eye, center, up);
    mat4x4_perspective(Projection, 45, vm.width/(float)vm.height, 1.0f, 100.0f);
    mat4x4_dup(MVPMatrix, Model);
    mat4x4_mul(MVPMatrix, View, MVPMatrix);
    mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
    MVPMatrixLoc = glGetUniformLocation(progid, "MVPMatrix");
    glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);

    glViewport(0, 0, vm.width, vm.height);
    mat4x4_perspective(Projection, 45, vm.width/(float)vm.height, 1.0f, 100.0f);
    mat4x4_dup(MVPMatrix, Model);
    mat4x4_mul(MVPMatrix, View, MVPMatrix);
    mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
    glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);


                            sfWindow_setFramerateLimit(window, 60);
                            sfWindow_setKeyRepeatEnabled(window, false);
                            fullscreen = !fullscreen;
                            break;
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

        mat4x4_identity(Model);
        mat4x4_look_at(View, eye, center, up);
        mat4x4_dup(MVPMatrix, Model);
        mat4x4_mul(MVPMatrix, View, MVPMatrix);
        mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
        glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);
    }    
    mousex = event.mouseMove.x;
    mousey = event.mouseMove.y;

                    break;
                case sfEvtMouseEntered:
                    break;
                case sfEvtMouseLeft: 
    mousein = false;
                    break;
                case sfEvtJoystickButtonPressed:
                case sfEvtJoystickButtonReleased:
                    printf("Joystick %d: Button %2d %s.\n", 
                            event.joystickButton.joystickId, 
                            event.joystickButton.button, 
                            event.type==sfEvtJoystickButtonPressed ? "pressed" : "released");
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
            mat4x4_identity(Model);
            mat4x4_look_at(View, eye, center, up);
            mat4x4_dup(MVPMatrix, Model);
            mat4x4_mul(MVPMatrix, View, MVPMatrix);
            mat4x4_mul(MVPMatrix, Projection, MVPMatrix);
            glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, &MVPMatrix[0][0]);
        }

        /* TODO traiter évènements */

    } /* end while(running) */
    Cube_free(&cube);
    glDeleteProgram(progid);
    sfWindow_destroy(window); /* Also performs close(), which deletes the OpenGL context. */
    sfContext_destroy(ctx);
    sfClock_destroy(clock);
    exit(EXIT_SUCCESS);
}
