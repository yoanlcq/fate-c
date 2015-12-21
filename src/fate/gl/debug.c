#include <stdio.h>
#include <stdbool.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/debug.h>

#ifdef FATE_GL_DEBUG

static void GLAPIENTRY fate_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam) {}
PFNGLDEBUGMESSAGECALLBACKPROC fate_glDebugMessageCallback = 
                              fate_glDebugMessageCallback_dummy;

static void GLAPIENTRY fate_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled) {}
PFNGLDEBUGMESSAGECONTROLPROC fate_glDebugMessageControl =
                             fate_glDebugMessageControl_dummy;

static void GLAPIENTRY fate_glDebugMessageInsertAMD(GLenum source, GLenum type, 
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
static void GLAPIENTRY fate_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message) {}
PFNGLDEBUGMESSAGEINSERTPROC fate_glDebugMessageInsert =
                            fate_glDebugMessageInsert_dummy;

static GLuint GLAPIENTRY fate_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog) { return 0; }
PFNGLGETDEBUGMESSAGELOGPROC fate_glGetDebugMessageLog =
                            fate_glGetDebugMessageLog_dummy;

static void GLAPIENTRY fate_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message) {}
PFNGLPUSHDEBUGGROUPPROC fate_glPushDebugGroup =
                        fate_glPushDebugGroup_dummy;

static void GLAPIENTRY fate_glPopDebugGroup_dummy(void) {}
PFNGLPOPDEBUGGROUPPROC fate_glPopDebugGroup =
                       fate_glPopDebugGroup_dummy;

static void GLAPIENTRY fate_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label) {}
PFNGLOBJECTLABELPROC fate_glObjectLabel =
                     fate_glObjectLabel_dummy;

static void GLAPIENTRY fate_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label) {}
PFNGLOBJECTPTRLABELPROC fate_glObjectPtrLabel =
                        fate_glObjectPtrLabel_dummy;

static void GLAPIENTRY fate_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label) {}
PFNGLGETOBJECTLABELPROC fate_glGetObjectLabel =
                        fate_glGetObjectLabel_dummy;

static void GLAPIENTRY fate_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label) {}
PFNGLGETOBJECTPTRLABELPROC fate_glGetObjectPtrLabel =
                           fate_glGetObjectPtrLabel_dummy;


/* The good news is that the ARB constants have the same value as their
 * core alias. That is, GL_DEBUG_SOURCE_API == GL_DEBUG_SOURCE_API_ARB. 
 *
 * So we can use this callback for both the KHR and ARB setups.
 * However we still get to write another version of this callback for
 * the AMD setup.
 */
void GLAPIENTRY fate_gl_debug_msg_callback(GLenum source, GLenum type, 
                                GLuint id, GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                const void* userParam)
{
    char *source_str;
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
    char *type_str;
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
    char *severity_str;
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:         severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          severity_str = "Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Info"; break;
    }

    fate_logf_video("OpenGL debug message "
                    "(severity:%s, type:%s, source:%s, id:%#08X) :\n"
                    "\t%s\n---\n",
                    severity_str, type_str, source_str, id, message);
}

void GLAPIENTRY fate_gl_debug_msg_callback_amd(GLuint id, GLenum category,
                                GLenum severity, 
                                GLsizei length, const GLchar* message, 
                                void* userParam)
{
    char *category_str;
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
    char *severity_str;
    switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH_AMD:   severity_str = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM_AMD: severity_str = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW_AMD:    severity_str = "Low"; break;
    }

    fate_logf_video("OpenGL debug message "
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

static void fgl_debug_reset_funcs(void) {
#define HELPER(_name_) \
    fate_##_name_ = fate_##_name_##_dummy
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

static void fgl_debug_setup_khr(bool enable) {

    if(!enable) {
        glDisable(GL_DEBUG_OUTPUT);
        fgl_debug_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glEnable(GL_DEBUG_OUTPUT);
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(fate_gl_debug_msg_callback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControl(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(_name_) \
    fate_##_name_ = _name_
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

static void fgl_debug_setup_arb(bool enable) {

    if(!enable) {
        fgl_debug_reset_funcs();
        return;
    } 

    const fgl_ign_message *ignmsg;
    
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(fate_gl_debug_msg_callback, NULL);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                          0, NULL, GL_TRUE);
    for(ignmsg = fgl_ign_messages ; ignmsg->id ; ++ignmsg) {
        glDebugMessageControlARB(ignmsg->source, ignmsg->type, GL_DONT_CARE,
                              1, &(ignmsg->id), GL_FALSE);
    }
#define HELPER(_name_) \
    fate_##_name_ = _name_##ARB
    HELPER(glDebugMessageCallback);
    HELPER(glDebugMessageControl);
    HELPER(glDebugMessageInsert);
    HELPER(glGetDebugMessageLog);
#undef HELPER
    if(GLEW_EXT_debug_label) {
        fate_glObjectLabel = glLabelObjectEXT;
        fate_glGetObjectLabel = glGetObjectLabelEXT;
    } else {
        fate_glObjectLabel = fate_glObjectLabel_dummy;
        fate_glGetObjectLabel = fate_glGetObjectLabel_dummy;
    }
#define HELPER(_name_) \
    fate_##_name_ = fate_##_name_##_dummy
    HELPER(glPushDebugGroup);
    HELPER(glPopDebugGroup);
    HELPER(glObjectPtrLabel);
    HELPER(glGetObjectPtrLabel);
#undef HELPER
}

static void fgl_debug_setup_amd(bool enable) {

    fgl_debug_reset_funcs();

    if(!enable)
        return;

    const fgl_ign_message_amd *ignmsg;

    glDebugMessageCallbackAMD(fate_gl_debug_msg_callback_amd, NULL);
    glDebugMessageEnableAMD(0, 0, 0, NULL, GL_TRUE);
    
    for(ignmsg = fgl_ign_messages_amd ; ignmsg->id ; ++ignmsg)
        glDebugMessageEnableAMD(ignmsg->category,0, 1, &(ignmsg->id),GL_FALSE);
    
    fate_glDebugMessageInsert = fate_glDebugMessageInsertAMD;
    
    if(GLEW_EXT_debug_label) {
        fate_glObjectLabel = glLabelObjectEXT;
        fate_glGetObjectLabel = glGetObjectLabelEXT;
    }
}

static void fgl_debug_insert_marker_dummy(const char *string) {}
static void fgl_debug_insert_marker_ext(const char *string) {
    glInsertEventMarkerEXT(0, string);
}
static void fgl_debug_insert_marker_gremedy(const char *string) {
    glStringMarkerGREMEDY(0, string);
}

void (*fate_gl_debug_insert_marker)(const char *string);

#endif /* FATE_GL_DEBUG */


void fate_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable) {

#ifdef FATE_GL_DEBUG
    if((gl_major>4 || (gl_major==4 && gl_minor>=3)) || GLEW_KHR_debug)
        fgl_debug_setup_khr(enable);
    else {
        if(GLEW_ARB_debug_output)
            fgl_debug_setup_arb(enable);
        else if(GLEW_AMD_debug_output)
            fgl_debug_setup_amd(enable);
        else enable = false;
    }
    if(GLEW_EXT_debug_marker)
        fate_gl_debug_insert_marker = fgl_debug_insert_marker_ext;
    else if(GLEW_GREMEDY_string_marker)
        fate_gl_debug_insert_marker = fgl_debug_insert_marker_gremedy;
    else
        fate_gl_debug_insert_marker = fgl_debug_insert_marker_dummy;
#else
    enable = false;
#endif
    fate_logf_video("OpenGL debug is %s.\n", enable ? "enabled" : "disabled");
}

