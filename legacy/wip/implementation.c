/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

#include <fst/implementation.h>
#include <fst/scheduler.h>
#include <fst/globaltree.h>
#include <opengl_base.h>
#include <opengl_debug.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

inline int fst_implementation_init(fst_implementation *impl)
{

    /* Check for compatibility issues */

    impl->dummy_context = sfContext_create();

    impl->context_settings.depthBits         = 24;
    impl->context_settings.stencilBits       = 8;
    impl->context_settings.antialiasingLevel = 0;
    impl->context_settings.majorVersion      = 4;
    impl->context_settings.minorVersion      = 3;
    impl->context_settings.attributeFlags    = sfContextCore;
#ifdef OPENGL_DEBUG
    impl->context_settings.attributeFlags   |= sfContextDebug;
#endif

    printf("--- OpenGL version ---\n"
           "    Required  : >= %d.%d\n"
           "    Supported :    %s\n"
           "\n"
           "--- OpenGL device ---\n"
           "    Renderer  : %s\n"
           "    Vendor    : %s\n"
           "\n",
           impl->context_settings.majorVersion, 
           impl->context_settings.minorVersion, 
           glGetString(GL_VERSION),
           glGetString(GL_RENDERER),
           glGetString(GL_VENDOR));

    GLint gl_version[2];
    glGetIntegerv(GL_MAJOR_VERSION, &gl_version[0]);
    glGetIntegerv(GL_MINOR_VERSION, &gl_version[1]);

    if(gl_version[0] < 4 && gl_version[1] < 3)
    {
        fprintf(stderr, 
                "Your hardware/driver combination does not meet the requirements.\n"
                "Make sure that the device used is the one you are expecting, "
                "and that your drivers are up-to-date.\n");
        return EXIT_FAILURE;
    }

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    if(glewInitResult != GLEW_OK)
    {
        fprintf(stderr, "Could not initialize GLEW :\n%s\n", 
                glewGetErrorString(glewInitResult));
        return EXIT_FAILURE;
    }

    fst_globaltree_init(&impl->globaltree);
    fst_scheduler_init(&impl->scheduler);
}
inline int fst_implementation_exec(fst_implementation *impl) 
{
    fst_globaltree_use(&impl->global_tree);

    sfVector2i window_position;
    sfVideoMode video_mode;

    window_position.x = fst_getk_u16("%.window.x")->value;
    window_position.y = fst_getk_u16("%.window.y")->value;
    video_mode.width  = fst_getk_u16("%.window.width")->value;
    video_mode.height = fst_getk_u16("%.window.height")->value;
    video_mode.bitsPerPixel = fst_getk_u8("%.window.bits_per_pixel")->value;

    impl->window = sfWindow_createUnicode(
            video_mode, fst_getk_str32("%.window.title")->value, 
            sfDefaultStyle, &impl->context_settings);

    /* FIXME This could be an issue. */
    sfContext_destroy(impl->dummy_context);

    sfWindow_setPosition(window_position);

    if(fst_getk_u8("%.window.vsync")->value) {
        sfWindow_setFramerateLimit(impl->window, 0);
        sfWindow_setVerticalSyncEnabled(impl->window, true);
    } else {
        sfWindow_setFramerateLimit(window, 
                fst_getk_u16("%.window.refresh_rate")->value);
        sfWindow_setVerticalSyncEnabled(impl->window, false);
    }

    sfWindow_setKeyRepeatEnabled(window, false);

    impl->context_settings = sfWindow_getSettings(window);

    fprintf(stdout,
            "--- Active OpenGL context settings ---\n"
            "    Version             : %d.%d\n"
            "    GLSL version        : %s\n"
            "    Depth buffer bits   : %d\n"
            "    Stencil buffer bits : %d\n"
            "    Antialiasing level  : %dx\n"
            "\n",
            impl->context_settings.majorVersion, 
            impl->context_settings.minorVersion,
            glGetString(GL_SHADING_LANGUAGE_VERSION),
            impl->context_settings.depthBits,
            impl->context_settings.stencilBits,
            impl->context_settings.antialiasingLevel);

#ifdef OPENGL_DEBUG
    glDebugMessageCallback((GLDEBUGPROC) &opengl_debug_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
#endif

    glClearColor(fst_getk_f32("%.window.clear_color.r")->value,
                 fst_getk_f32("%.window.clear_color.g")->value,
                 fst_getk_f32("%.window.clear_color.b")->value,
                 fst_getk_f32("%.window.clear_color.a")->value);

    sfEvent event;
    while(fst_scheduler_next_frame(impl->scheduler, impl->window))
        while(sfWindow_pollEvent(impl->window, &event))
            fst_scheduler_handle_event(impl->scheduler, event);

    int exit_status = fst_scheduler_exit_status(impl->scheduler);

    /* The Scheduler and Global Tree must be destroyed before 
     * OpenGL context deletion (i.e before the window is closed). */
    fst_scheduler_deinit(&impl->scheduler);
    fst_globaltree_deinit(&impl->globaltree);
    sfWindow_close(impl->window);
    sfWindow_destroy(impl->window);

    return exit_status;
}
