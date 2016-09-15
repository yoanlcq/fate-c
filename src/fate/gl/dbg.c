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

/*! \file fate/gl/dbg.c
 *  \brief OpenGL debug
 *
 * TODO
 */

#include <stdio.h>
#include <stdbool.h>
#include <fate/log.h>
#include <fate/gl.h>



#ifdef FE_GL_DBG
static const char *TAG = "fe_gl_dbg";


static void GLAPIENTRY fe_gl_dbg_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam) {}
PFNGLDEBUGMESSAGECALLBACKPROC fe_gl_dbg_glDebugMessageCallback = 
                              fe_gl_dbg_glDebugMessageCallback_dummy;

static void GLAPIENTRY fe_gl_dbg_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled) {}
PFNGLDEBUGMESSAGECONTROLPROC fe_gl_dbg_glDebugMessageControl =
                             fe_gl_dbg_glDebugMessageControl_dummy;

static void GLAPIENTRY fe_gl_dbg_glDebugMessageInsertAMD(GLenum source, GLenum type, 
                                    GLuint id, GLenum severity, GLsizei length, 
                                    const char *message) 
{
    GLenum category;
    switch(source) {
    case GL_DEBUG_SOURCE_API: 
        category = GL_DEBUG_CATEGORY_API_ERROR_AMD;
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: 
        category = GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD;
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: 
        category = GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD;
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        category = GL_DEBUG_CATEGORY_APPLICATION_AMD;
        break;
    default:
        switch(type) {
        case GL_DEBUG_TYPE_ERROR: 
            category = GL_DEBUG_CATEGORY_API_ERROR_AMD;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
            category = GL_DEBUG_CATEGORY_DEPRECATION_AMD;
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: 
            category = GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD;
            break;
        case GL_DEBUG_TYPE_PERFORMANCE: 
            category = GL_DEBUG_CATEGORY_PERFORMANCE_AMD;
            break;
        default: category = GL_DEBUG_CATEGORY_OTHER_AMD; break;
        }
        break;
    }

    if(severity==GL_DEBUG_SEVERITY_NOTIFICATION)
        severity = GL_DEBUG_SEVERITY_LOW_AMD;

    glDebugMessageInsertAMD(category, severity, id, length, message);
}
static void GLAPIENTRY fe_gl_dbg_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message) {}
PFNGLDEBUGMESSAGEINSERTPROC fe_gl_dbg_glDebugMessageInsert =
                            fe_gl_dbg_glDebugMessageInsert_dummy;

static GLuint GLAPIENTRY fe_gl_dbg_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog) { return 0; }
PFNGLGETDEBUGMESSAGELOGPROC fe_gl_dbg_glGetDebugMessageLog =
                            fe_gl_dbg_glGetDebugMessageLog_dummy;

static void GLAPIENTRY fe_gl_dbg_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message) {}
PFNGLPUSHDEBUGGROUPPROC fe_gl_dbg_glPushDebugGroup =
                        fe_gl_dbg_glPushDebugGroup_dummy;

static void GLAPIENTRY fe_gl_dbg_glPopDebugGroup_dummy(void) {}
PFNGLPOPDEBUGGROUPPROC fe_gl_dbg_glPopDebugGroup =
                       fe_gl_dbg_glPopDebugGroup_dummy;

static void GLAPIENTRY fe_gl_dbg_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label) {}
PFNGLOBJECTLABELPROC fe_gl_dbg_glObjectLabel =
                     fe_gl_dbg_glObjectLabel_dummy;

static void GLAPIENTRY fe_gl_dbg_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label) {}
PFNGLOBJECTPTRLABELPROC fe_gl_dbg_glObjectPtrLabel =
                        fe_gl_dbg_glObjectPtrLabel_dummy;

static void GLAPIENTRY fe_gl_dbg_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label) {}
PFNGLGETOBJECTLABELPROC fe_gl_dbg_glGetObjectLabel =
                        fe_gl_dbg_glGetObjectLabel_dummy;

static void GLAPIENTRY fe_gl_dbg_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label) {}
PFNGLGETOBJECTPTRLABELPROC fe_gl_dbg_glGetObjectPtrLabel =
                           fe_gl_dbg_glGetObjectPtrLabel_dummy;


/* The good news is that the ARB constants have the same value as their
 * core alias. That is, GL_DEBUG_SOURCE_API == GL_DEBUG_SOURCE_API_ARB. 
 *
 * So we can use this callback for both the KHR and ARB setups.
 * However we still get to write another version of this callback for
 * the AMD setup.
 */
void GLAPIENTRY fe_gl_dbg_gl_debug_msg_callback(GLenum source, GLenum type, 
                                GLuint id, GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                const void* userParam)
{
    char *source_str="";
    switch(source) {
    case GL_DEBUG_SOURCE_API: 
        source_str = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: 
        source_str = "Window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: 
        source_str = "Shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: 
        source_str = "3rd party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_str = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER:
        source_str = "Other"; break;
    }
    char *type_str="";
    switch(type) {
    case GL_DEBUG_TYPE_ERROR: 
        type_str = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: 
        type_str = "Deprecated behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: 
        type_str = "Undefined behaviour"; break;
    case GL_DEBUG_TYPE_PERFORMANCE: 
        type_str = "Performance"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        type_str = "Portability"; break;
    case GL_DEBUG_TYPE_MARKER:
        type_str = "Command stream annotation"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        type_str = "Push debug group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        type_str = "Pop debug group"; break;
    case GL_DEBUG_TYPE_OTHER:
        type_str = "Other"; break;
    }
    char *severity_str="";
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:         severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          severity_str = "Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Info"; break;
    }

    fe_logd(TAG, "OpenGL debug message "
                    "(severity:%s, type:%s, source:%s, id:%#08X) :\n"
                    "\t%s\n---\n",
                    severity_str, type_str, source_str, id, message);
}

void GLAPIENTRY fe_gl_dbg_gl_debug_msg_callback_amd(GLuint id, GLenum category,
                                GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                void* userParam)
{
    char *category_str="";
    switch(category) {
    case GL_DEBUG_CATEGORY_API_ERROR_AMD: 
        category_str = "API Error";
        break;
    case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
        category_str = "Window system";
        break;
    case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
        category_str = "Deprecated behaviour";
        break;
    case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
        category_str = "Undefined behaviour";
        break;
    case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
        category_str = "Performance";
        break;
    case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
        category_str = "Shader compiler";
        break;
    case GL_DEBUG_CATEGORY_APPLICATION_AMD:
        category_str = "Application";
        break;
    case GL_DEBUG_CATEGORY_OTHER_AMD:
        category_str = "Other";
        break;
    }
    char *severity_str="";
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_AMD:   severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM_AMD: severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW_AMD:    severity_str = "Low"; break;
    }

    fe_logd(TAG, "OpenGL debug message "
                    "(severity:%s, category:%s, id:%#08X) :\n"
                    "\t%s\n---\n",
                    severity_str, category_str, id, message);
}

struct fgl_ign_message {
    GLenum source;
    GLenum type;
    GLuint id;
};
typedef struct fgl_ign_message fgl_ign_message;

static const fgl_ign_message fgl_ign_messages[] = {
    /* From the NVidia driver, this message keeps popping up when 
     * calling glClear() while GL_SCISSOR_TEST is enabled. */
    { GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, 0x020043 },
    /* A null id indicates the end of the array. */
    { GL_DONT_CARE, GL_DONT_CARE, 0 }
};

struct fgl_ign_message_amd {
    GLenum category;
    GLuint id;
};
typedef struct fgl_ign_message_amd fgl_ign_message_amd;

static const fgl_ign_message_amd fgl_ign_messages_amd[] = {
    /* Fill here as you wish, but please keep the last element. */
    /* A null id indicates the end of the array. */
    { 0, 0 }
};

static void fgl_dbg_reset_funcs(void) {
#define HELPER(glFoo) \
    fe_gl_dbg_##glFoo = fe_gl_dbg_##glFoo##_dummy
    HELPER(glDebugMessageCallback); 
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert); 
    HELPER(glGetDebugMessageLog); 
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectLabel);
    HELPER(glObjectPtrLabel); 
    HELPER(glGetObjectLabel); 
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_khr(bool enable) {

    if(!enable) {
        glDisable(GL_DEBUG_OUTPUT);
        fgl_dbg_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(fe_gl_dbg_gl_debug_msg_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControl(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(glFoo) \
    fe_gl_dbg_##glFoo = glFoo
    HELPER(glDebugMessageCallback);
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert);
    HELPER(glGetDebugMessageLog);
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectLabel);
    HELPER(glObjectPtrLabel);
    HELPER(glGetObjectLabel);
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_arb(bool enable) {

    if(!enable) {
        fgl_dbg_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(fe_gl_dbg_gl_debug_msg_callback, NULL);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControlARB(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(glFoo) \
    fe_gl_dbg_##glFoo = glFoo##ARB
    HELPER(glDebugMessageCallback);
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert);
    HELPER(glGetDebugMessageLog);
#undef HELPER
    if(GLAD_GL_EXT_debug_label) {
        fe_gl_dbg_glObjectLabel = glLabelObjectEXT;
        fe_gl_dbg_glGetObjectLabel = glGetObjectLabelEXT;
    } else {
        fe_gl_dbg_glObjectLabel = fe_gl_dbg_glObjectLabel_dummy;
        fe_gl_dbg_glGetObjectLabel = fe_gl_dbg_glGetObjectLabel_dummy;
    }
#define HELPER(glFoo) \
    fe_gl_dbg_##glFoo = fe_gl_dbg_##glFoo##_dummy
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectPtrLabel);
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_dbg_setup_amd(bool enable) {

    fgl_dbg_reset_funcs();

    if(!enable)
        return;

    const fgl_ign_message_amd *ignmsg;

    glDebugMessageCallbackAMD(fe_gl_dbg_gl_debug_msg_callback_amd, NULL);
    glDebugMessageEnableAMD(0, 0, 0, NULL, GL_TRUE);
    
    for(ignmsg = fgl_ign_messages_amd ; ignmsg->id ; ++ignmsg)
        glDebugMessageEnableAMD(ignmsg->category,0, 1, &(ignmsg->id),GL_FALSE);
    
    fe_gl_dbg_glDebugMessageInsert = fe_gl_dbg_glDebugMessageInsertAMD;
    
    if(GLAD_GL_EXT_debug_label) {
        fe_gl_dbg_glObjectLabel = glLabelObjectEXT;
        fe_gl_dbg_glGetObjectLabel = glGetObjectLabelEXT;
    }
}

static void fgl_dbg_insert_marker_dummy(const char *string) {}
static void fgl_dbg_insert_marker_ext(const char *string) {
    glInsertEventMarkerEXT(0, string);
}
static void fgl_dbg_insert_marker_gremedy(const char *string) {
    glStringMarkerGREMEDY(0, string);
}

void (*fe_gl_dbg_insert_marker)(const char *string);

#endif /* FE_GL_DBG */


void fe_gl_dbg_setup(const fe_gl_version *v, bool enable) {

    /* XXX Does this work on OpenGL ES ? 
     * It should not. On ES, we must suffix everything from GL_KHR_debug 
     * with KHR. */
#ifdef FE_GL_DBG
    bool khr_supported = (v->es 
        ? v->major > 3 || (v->major == 3 && v->minor >= 2)
        : v->major > 4 || (v->major == 4 && v->minor >= 3) || GLAD_GL_KHR_debug
    );
    if(khr_supported)
        fgl_dbg_setup_khr(enable);
    else {
        if(GLAD_GL_ARB_debug_output)
            fgl_dbg_setup_arb(enable);
        else if(GLAD_GL_AMD_debug_output)
            fgl_dbg_setup_amd(enable);
        else enable = false;
    }
    if(GLAD_GL_EXT_debug_marker)
        fe_gl_dbg_insert_marker = fgl_dbg_insert_marker_ext;
    else if(GLAD_GL_GREMEDY_string_marker)
        fe_gl_dbg_insert_marker = fgl_dbg_insert_marker_gremedy;
    else
        fe_gl_dbg_insert_marker = fgl_dbg_insert_marker_dummy;
#else
    enable = false;
#endif
    fe_logd(TAG, "OpenGL debug is %s.\n", enable ? "enabled" : "disabled");
}

