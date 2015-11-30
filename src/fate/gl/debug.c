#include <stdio.h>
#include <fate/log.h>
#include <fate/gl/defs.h>
#include <fate/gl/debug.h>

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



void fate_glDebugMessageCallback_dummy(GLDEBUGPROC callback, 
                                       const void *userParam) {}
PFNGLDEBUGMESSAGECALLBACKPROC fate_glDebugMessageCallback;

void fate_glDebugMessageControl_dummy(GLenum source, GLenum type, 
                                      GLenum severity, GLsizei count, 
                                      const GLuint *ids, 
                                      GLboolean enabled) {}
PFNGLDEBUGMESSAGECONTROLPROC fate_glDebugMessageControl;

void fate_glDebugMessageInsert_dummy(GLenum source, GLenum type, GLuint id, 
                                    GLenum severity, GLsizei length, 
                                    const char *message) {}
PFNGLDEBUGMESSAGEINSERTPROC fate_glDebugMessageInsert;

GLuint fate_glGetDebugMessageLog_dummy(GLuint count, GLsizei bufSize, 
                                     GLenum *sources, GLenum *types, 
                                     GLuint *ids, GLenum *severities,
                                     GLsizei *lengths, 
                                     GLchar *messageLog) { return 0; }
PFNGLGETDEBUGMESSAGELOGPROC fate_glGetDebugMessageLog;

void fate_glPushDebugGroup_dummy(GLenum source, GLuint id, GLsizei length, 
                                const char *message) {}
PFNGLPUSHDEBUGGROUPPROC fate_glPushDebugGroup;

void fate_glPopDebugGroup_dummy(void) {}
PFNGLPOPDEBUGGROUPPROC fate_glPopDebugGroup;

void fate_glObjectLabel_dummy(GLenum identifier, GLuint name, GLsizei length, 
                             const char *label) {}
PFNGLOBJECTLABELPROC fate_glObjectLabel;

void fate_glObjectPtrLabel_dummy(const void *ptr, GLsizei length, 
                                const GLchar *label) {}
PFNGLOBJECTPTRLABELPROC fate_glObjectPtrLabel;

void fate_glGetObjectLabel_dummy(GLenum identifier, GLuint name, 
                                GLsizei bufSize,
                                GLsizei *length, char *label) {}
PFNGLGETOBJECTLABELPROC fate_glGetObjectLabel;

void fate_glGetObjectPtrLabel_dummy(const void *ptr, GLsizei bufSize, 
                                   GLsizei *length, char *label) {}
PFNGLGETOBJECTPTRLABELPROC fate_glGetObjectPtrLabel;


void fate_gl_debug_setup(GLint gl_major, GLint gl_minor, bool enable) {
    (enable ? glEnable : glDisable)(GL_DEBUG_OUTPUT);
    /* The variable is used later. */
    int can_debug = !(gl_major < 4 || (gl_major == 4 && gl_minor < 3));
    if(can_debug && enable) {
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
        fate_glDebugMessageCallback = 
            (PFNGLDEBUGMESSAGECALLBACKPROC) fate_glDebugMessageCallback_dummy;
        fate_glDebugMessageControl = 
            (PFNGLDEBUGMESSAGECONTROLPROC) fate_glDebugMessageControl_dummy;
        fate_glDebugMessageInsert = 
            (PFNGLDEBUGMESSAGEINSERTPROC) fate_glDebugMessageInsert_dummy;
        fate_glGetDebugMessageLog = 
            (PFNGLGETDEBUGMESSAGELOGPROC) fate_glGetDebugMessageLog_dummy;
        fate_glPushDebugGroup =
            (PFNGLPUSHDEBUGGROUPPROC) fate_glPushDebugGroup_dummy;
        fate_glPopDebugGroup = 
            (PFNGLPOPDEBUGGROUPPROC) fate_glPopDebugGroup_dummy;
        fate_glObjectLabel = 
            (PFNGLOBJECTLABELPROC) fate_glObjectLabel_dummy;
        fate_glObjectPtrLabel = 
            (PFNGLOBJECTPTRLABELPROC) fate_glObjectPtrLabel_dummy;
        fate_glGetObjectLabel = 
            (PFNGLGETOBJECTLABELPROC) fate_glGetObjectLabel_dummy;
        fate_glGetObjectPtrLabel = 
            (PFNGLGETOBJECTPTRLABELPROC) fate_glGetObjectPtrLabel_dummy;
    }
    fate_logf_video("OpenGL debug functions are now turned %s.\n", 
                    can_debug ? "on" : "off");
}

#endif /* FATE_GL_DEBUG */
