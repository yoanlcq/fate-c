#include <fate/gl/defs.h>
#include <fate/gl/cube.h>

void Cube_init(Cube *c) 
{
    c->vertices[0][0] =  127;
    c->vertices[0][1] = -127;
    c->vertices[0][2] = -127;
    c->vertices[1][0] =  127;
    c->vertices[1][1] =  127;
    c->vertices[1][2] = -127;
    c->vertices[2][0] = -127;
    c->vertices[2][1] = -127;
    c->vertices[2][2] = -127;
    c->vertices[3][0] = -127;
    c->vertices[3][1] =  127;
    c->vertices[3][2] = -127;
    c->vertices[4][0] = -127;
    c->vertices[4][1] = -127;
    c->vertices[4][2] =  127;
    c->vertices[5][0] = -127;
    c->vertices[5][1] =  127;
    c->vertices[5][2] =  127;
    c->vertices[6][0] =  127;
    c->vertices[6][1] = -127;
    c->vertices[6][2] =  127;
    c->vertices[7][0] =  127;
    c->vertices[7][1] =  127;
    c->vertices[7][2] =  127;
    c->colors[0][0] = 255;
    c->colors[0][1] = 0;
    c->colors[0][2] = 0;
    c->colors[1][0] = 255;
    c->colors[1][1] = 0;
    c->colors[1][2] = 0;
    c->colors[2][0] = 0;
    c->colors[2][1] = 255;
    c->colors[2][2] = 0;
    c->colors[3][0] = 0;
    c->colors[3][1] = 255;
    c->colors[3][2] = 0;
    c->colors[4][0] = 0;
    c->colors[4][1] = 0;
    c->colors[4][2] = 255;
    c->colors[5][0] = 0;
    c->colors[5][1] = 0;
    c->colors[5][2] = 255;
    c->colors[6][0] = 255;
    c->colors[6][1] = 255;
    c->colors[6][2] = 0;
    c->colors[7][0] = 255;
    c->colors[7][1] = 255;
    c->colors[7][2] = 0;
    c->indices[0]  = 0;
    c->indices[1]  = 1;
    c->indices[2]  = 2;
    c->indices[3]  = 3;
    c->indices[4]  = 4;
    c->indices[5]  = 5;
    c->indices[6]  = 6;
    c->indices[7]  = 7;
    c->indices[8]  = 0xFF;
    c->indices[9]  = 5;
    c->indices[10] = 3;
    c->indices[11] = 7;
    c->indices[12] = 1;
    c->indices[13] = 6;
    c->indices[14] = 0;
    c->indices[15] = 4;
    c->indices[16] = 2;

    glGenBuffers(1, &c->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(c->indices), c->indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &c->vao);
    glBindVertexArray(c->vao);
 
    glGenBuffers(1, &c->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(c->vertices)+sizeof(c->colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(c->vertices), c->vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(c->vertices), sizeof(c->colors), c->colors);

    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, BUFFER_OFFSET(sizeof(c->vertices)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}
void Cube_free(Cube *c) {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &c->ebo);
    glDeleteBuffers(1, &c->vbo);
    glDeleteVertexArrays(1, &c->vao);
}
void Cube_draw(Cube *c) 
{
    glBindVertexArray(c->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->ebo);
    glPrimitiveRestartIndex(0xFF);
    glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_BYTE, NULL);
}


