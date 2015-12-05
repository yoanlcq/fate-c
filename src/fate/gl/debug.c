#include <stdio.h>
#include <stdbool.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/debug.h>

#ifdef FATE_GL_DEBUG

struct fgl_ign_message {
    GLenum source;
    GLenum type;
    GLuint id;
};
typedef struct fgl_ign_message fgl_ign_message;

#endif /* FATE_GL_DEBUG */

void fate_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable) {
    bool can_debug = (gl_major > 4 || (gl_major == 4 && gl_minor >= 3));

#ifdef FATE_GL_DEBUG

    const fgl_ign_message fgl_ign_messages[] = {
        /* From the NVidia driver, this message keeps popping up when 
         * calling glClear() while GL_SCISSOR_TEST is enabled. */
        { GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, 0x020043 },
        /* A null id indicates the end of the array. */
        { GL_DONT_CARE, GL_DONT_CARE, 0 }
    };
    const fgl_ign_message *ignmsg;

    if(can_debug && !enable) {
        glDisable(GL_DEBUG_OUTPUT);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    } 
    
    if(can_debug && enable) {
        glEnable(GL_DEBUG_OUTPUT);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(fate_gl_debug_msg_callback, NULL);
        /* Enable all debug messages by default */
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 
                              0, NULL, GL_TRUE);
        /* Ignore some */
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
    } else {
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
#endif /* FATE_GL_DEBUG */
    fate_logf_video("OpenGL debug functions are turned %s.\n", 
                    can_debug && enable ? "on" : "off");
}



#ifdef FATE_GL_DEBUG

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
            source_str = "window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: 
            source_str = "shader compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: 
            source_str = "3rd party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:
            source_str = "application"; break;
        case GL_DEBUG_SOURCE_OTHER:
            source_str = "other"; break;
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
            type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_str = "Push debug group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_str = "Pop debug group"; break;
        case GL_DEBUG_TYPE_OTHER:
            type_str = "Other"; break;

    }
    char *severity_str;
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: 
            severity_str = "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM: 
            severity_str = "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW: 
            severity_str = "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: 
            severity_str = "Info";
            break;
    }

    fate_logf_video("OpenGL debug message "
                    "(severity:%s, type:%s, source:%s, id:%#08X) :\n"
                    "\t%s\n---\n",
                    severity_str, type_str, source_str, id, message);
}



void GLAPIENTRY fate_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam) {}
PFNGLDEBUGMESSAGECALLBACKPROC fate_glDebugMessageCallback = 
                              fate_glDebugMessageCallback_dummy;

void GLAPIENTRY fate_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled) {}
PFNGLDEBUGMESSAGECONTROLPROC fate_glDebugMessageControl =
                             fate_glDebugMessageControl_dummy;

void GLAPIENTRY fate_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message) {}
PFNGLDEBUGMESSAGEINSERTPROC fate_glDebugMessageInsert =
                            fate_glDebugMessageInsert_dummy;

GLuint GLAPIENTRY fate_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog) { return 0; }
PFNGLGETDEBUGMESSAGELOGPROC fate_glGetDebugMessageLog =
                            fate_glGetDebugMessageLog_dummy;

void GLAPIENTRY fate_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message) {}
PFNGLPUSHDEBUGGROUPPROC fate_glPushDebugGroup =
                        fate_glPushDebugGroup_dummy;

void GLAPIENTRY fate_glPopDebugGroup_dummy(void) {}
PFNGLPOPDEBUGGROUPPROC fate_glPopDebugGroup =
                       fate_glPopDebugGroup_dummy;

void GLAPIENTRY fate_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label) {}
PFNGLOBJECTLABELPROC fate_glObjectLabel =
                     fate_glObjectLabel_dummy;

void GLAPIENTRY fate_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label) {}
PFNGLOBJECTPTRLABELPROC fate_glObjectPtrLabel =
                        fate_glObjectPtrLabel_dummy;

void GLAPIENTRY fate_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label) {}
PFNGLGETOBJECTLABELPROC fate_glGetObjectLabel =
                        fate_glGetObjectLabel_dummy;

void GLAPIENTRY fate_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label) {}
PFNGLGETOBJECTPTRLABELPROC fate_glGetObjectPtrLabel =
                           fate_glGetObjectPtrLabel_dummy;

#endif /* FATE_GL_DEBUG */
