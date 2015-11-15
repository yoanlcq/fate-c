#include <opengl_debug.h>
#include <stdio.h>

#ifdef OPENGL_DEBUG

void APIENTRY opengl_debug_callback  (GLenum source, 
                                      GLenum type, 
                                      GLuint id,
                                      GLenum severity, 
                                      GLsizei length, 
                                      const GLchar* message, 
                                      void* userParam)
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
            severity_str = "Notification";
            break;
    }
   
    fprintf(stderr, 
            "--- Debug message from OpenGL ---\n"
            "    Source   : %s\n"
            "    Type     : %s\n"
            "    ID       : 0x%08X\n"
            "    Severity : %s\n"
            "%s\n"
            "--- End of message ---\n"
            "\n",
            source_str, type_str, id, severity_str, message);
}

#endif


