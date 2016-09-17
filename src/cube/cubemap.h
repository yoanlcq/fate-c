#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <fate/gl.h>

GLuint cubemap_build_6cols(void);
/* http://opengameart.org/content/mayhems-skyboxes-more */
GLuint cubemap_build_grouse(void);

#endif /* CUBEMAP_H */
