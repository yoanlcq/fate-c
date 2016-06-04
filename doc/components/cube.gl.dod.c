struct cb_cube {
    GLuint ebo, vao, vbo;
};

void cb_cube_init(cb_cube c, GLuint prog) {
    GLbyte vertices[24] = { 
         127, -127, -127,
         127,  127, -127,
        -127, -127, -127,
        -127,  127, -127,
        -127, -127,  127,
        -127,  127,  127,
         127, -127,  127,
         127,  127,  127
    };
    GLubyte colors[24] = {
        255, 0, 0,
        255, 0, 0,
        0, 255, 0,
        0, 255, 0,
        0, 0, 255,
        0, 0, 255,
        255, 255, 0,
        255, 255, 0
    };
    GLubyte indices[17] = {
        0, 1, 2, 3, 4, 5, 6, 7,
        0xFF,
        5, 3, 7, 1, 6, 0, 4, 2
    };

    c..prog = prog;
    glUseProgram(c..prog);

    fe_glCreateBuffers(1, &c..ebo); //glGenBuffers(1, &c..ebo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c..ebo);
    fe_glObjectLabel(GL_BUFFER, c..ebo, -1, "\"cb_cube EBO\"");
    fe_glNamedBufferData(c..ebo, GL_ELEMENT_ARRAY_BUFFER, 
                         sizeof(indices), indices, GL_STATIC_DRAW);
 
    fe_glCreateBuffers(1, &c..vbo); //glGenBuffers(1, &c..vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, c..vbo);
    fe_glObjectLabel(GL_BUFFER, c..vbo, -1, "\"cb_cube VBO\"");
    fe_glNamedBufferData(c..vbo, GL_ARRAY_BUFFER, 
                         sizeof(vertices)+sizeof(colors), 
                         NULL, GL_STATIC_DRAW);
    fe_glNamedBufferSubData(c..vbo, GL_ARRAY_BUFFER, 0, 
                            sizeof(vertices), vertices);
    fe_glNamedBufferSubData(c..vbo, GL_ARRAY_BUFFER, 
                            sizeof(vertices), sizeof(colors), colors);

    fe_glCreateVertexArrays(1, &c..vao); //glGenVertexArrays(1, &c..vao);
    //glBindVertexArray(c..vao);
    fe_glObjectLabel(GL_VERTEX_ARRAY, c..vao, -1, "\"cb_cube VAO\"");
    fe_glEnableVertexArrayAttrib(c..vao, 0); //glEnableVertexAttribArray(0);
    fe_glEnableVertexArrayAttrib(c..vao, 1); //glEnableVertexAttribArray(1);
    fe_glVertexArrayVertexBuffer(c..vao, 0, c..vbo, 0, 0);
    fe_glVertexArrayAttribFormat(c..vao, 0, 3, GL_BYTE, GL_TRUE, 0);
    fe_glVertexArrayAttribFormat(c..vao, 1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 
                                 BUFFER_OFFSET(sizeof(vertices)));
    /*
    glVertexAttribPointer(0, 3, GL_BYTE, GL_TRUE, 0, NULL);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 
                          BUFFER_OFFSET(sizeof(vertices)));
    */
    fe_glVertexArrayAttribBinding(c..vao, 0, 0);
    fe_glVertexArrayAttribBinding(c..vao, 1, 1);
    fe_glVertexArrayElementBuffer(c..vao, c..ebo);
}
void cb_cube_deinit(cb_cube c) {
    fe_glDisableVertexArrayAttrib(c..vao, 0); //glDisableVertexAttribArray(0);
    fe_glDisableVertexArrayAttrib(c..vao, 1); //glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &c..ebo);
    glDeleteBuffers(1, &c..vbo);
    glDeleteVertexArrays(1, &c..vao);
}
void cb_cube_draw(cb_cube c) {
    //glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE_STRIP);
    glCullFace(GL_FRONT);
    glUseProgram(c..prog);
    glBindVertexArray(c..vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c..ebo); //useless ??
    glPrimitiveRestartIndex(0xFF);
    /* Two strips + restart index. */
    glDrawElements(GL_TRIANGLE_STRIP, 8+8+1, GL_UNSIGNED_BYTE, NULL);
}


struct cb_demo {
    fe_window window;
    .....
};

void cb_demo_init_cubes(cb_demo demo) {
    GLuint progid = glCreateProgram();
    if(!fe_gl_mkprog(progid,
                       "data/OpenGL/triangles.glpb",
                       "res/shaders/triangles.330.core.vert",
                       "res/shaders/triangles.330.core.frag",
                       NULL))
        fe_fatal("Could not build the OpenGL program.\n");
    fe_gl_mkprog_cleanup();
    fe_glObjectLabel(GL_PROGRAM, progid, -1, "\"Cube program\"");
    demo..cube....alloc();
    cb_cube_init(demo..cube, progid);

    glEnable(GL_PRIMITIVE_RESTART); /* Since OpenGL 3.1 */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    demo..distance = 6.0f;
    demo..eye[0] = 0.0f;
    demo..eye[1] = 0.0f;
    demo..eye[2] = -demo..distance;
    memset(demo..center, 0, sizeof(fe_vec3));
    demo..up[0] = 0.0f;
    demo..up[1] = 1.0f;
    demo..up[2] = 0.0f;
    demo..MVPMatrixLoc = glGetUniformLocation(demo..cube..prog, "MVPMatrix");
    demo..ufInvertLoc  = glGetUniformLocation(demo..cube..prog, "ufInvert");

#define UPDATE_VIEW() \
    mat4_identity(demo..Model); \
    mat4_look_at(demo..View, demo..eye, demo..center, demo..up)

#define UPDATE_MVP() \
    mat4_mul(demo..MVPMatrix, demo..Projection, demo..View); \
    mat4_mul(demo..MVPMatrix, demo..Model, demo..MVPMatrix); \
    glUniformMatrix4fv(demo..MVPMatrixLoc, 1, GL_FALSE, &demo..MVPMatrix[0][0])
   
#define FE_FALLBACK_REFRESH_RATE 60
#define FE_DEFAULT_FPS_CEIL 256
#define FE_DEFAULT_NEAR 0.25
#define FE_DEFAULT_FAR  1024.0
#define FE_DEFAULT_FOV  (75.0*M_PI/180.0)

#define RESIZE(_W_,_H_) \
    demo..window..resize(fe_vec2_from_xy(_W_,_H_)); \
    mat4_perspective(demo..Projection, FE_DEFAULT_FOV, _W_/(float)_H_, \
                     FE_DEFAULT_NEAR, FE_DEFAULT_FAR); \
    UPDATE_MVP()

    UPDATE_VIEW();
    RESIZE(demo..win_w, demo..win_h);

    demo..mousex = demo..mousey = 0;
    demo..mousein = demo..mousedown = false;
    demo..h_angle = demo..v_angle = 0.0f;
    demo..R_x = demo..R_y = 0.0f;
    demo..zoom_in = demo..zoom_out = false;

    demo..go_west = demo..go_east = false;
    demo..splitx = 0.5f;

    demo..frameno = 0;
}

void cb_demo_clean_then_exit(void *data) {
    cb_demo demo = (cb_demo) data;
    glDeleteProgram(demo..prog);
    cb_cube_deinit(demo..cube);
    fe_exit(EXIT_SUCCESS);
}

void cb_demo_onframe(fe_taskctx *ctx) {
    cb_demo demo = (cb_demo) ctx->arg;
    bool dirty = false;
    demo..go_west  = demo..window..input.left.held;
    demo..go_east  = demo..window..input.right.held;
    demo..zoom_in  = demo..window..input.kp_plus.held;
    demo..zoom_out = demo..window..input.kp_minus.held;
    if(demo..window..input..f11..was_pressed)
        demo..window..toggle_fullscreen();
    if(demo..window..input.mouse->left.held) {
        demo..h_angle += (demo..window..input.mouse->move.x)*M_PI/180.0f;
        demo..v_angle += (demo..window..input.mouse->move.y)*M_PI/180.0f;
        if(demo..v_angle >= M_PI/2)
            demo..v_angle = M_PI/2 - M_PI/180.0f;
        else if(demo..v_angle <= -M_PI/2)
            demo..v_angle = -M_PI/2 + M_PI/180.0f;
        demo..eye[0] =  demo..distance*sinf(demo..h_angle)*cosf(demo..v_angle);
        demo..eye[1] =  demo..distance*sinf(demo..v_angle);
        demo..eye[2] = -demo..distance*cosf(demo..h_angle)*cosf(demo..v_angle);

        UPDATE_VIEW();
        UPDATE_MVP();
    }
    demo..mousex = demo..window..input.mouse->pos.x;
    demo..mousey = demo..window..input.mouse->pos.y;

    
    demo..distance -= demo..window..input.mouse.wheel.offset.y;

    if(demo..go_west && demo..splitx > 0.0025f)
        demo..splitx -= 0.0025f;
    if(demo..go_east && demo..splitx < 0.9975f)
        demo..splitx += 0.0025f;
    if(demo..R_x > 10.0f || demo..R_x < -10.0f || demo..R_y>10.0f || demo..R_y<-10.0f) {
        demo..eye[0] += demo..R_x/200.0f;
        demo..eye[1] += demo..R_y/200.0f;
        dirty = true;
    }
    if(demo..zoom_in) {
        demo..distance -= 0.25f;
        dirty = true;
    } else if(demo..zoom_out) {
        demo..distance += 0.25f;
        dirty = true;
    }

    demo..eye[0] =  demo..distance*sinf(demo..h_angle)*cosf(demo..v_angle);
    demo..eye[1] =  demo..distance*sinf(demo..v_angle);
    demo..eye[2] = -demo..distance*cosf(demo..h_angle)*cosf(demo..v_angle);
    UPDATE_VIEW();
    UPDATE_MVP();


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    glUseProgram(demo..prog);

    glScissor(0, 0, demo..splitx*(float)demo..win_w, demo..win_h);
    glClearColor(0.3f, 0.9f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1ui(demo..ufInvertLoc, 0);
    cb_cube_draw(demo..cube);

    glScissor(demo..splitx*(float)demo..win_w, 0, demo..win_w-demo..splitx*(float)demo..win_w, demo..win_h);
    glClearColor(0.7f, 0.1f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1ui(demo..ufInvertLoc, 1);
    cb_cube_draw(demo..cube);
    
    glDisable(GL_SCISSOR_TEST); 

    demo..window..gl..swap_buffers();
}

void cb_demo_init(cb_demo demo) {
    demo..window....alloc();
    demo..window..reset();
    demo..window..resize(fe_vec2_from_xy(800, 600));
    demo..window..set_icon(CB_RES_ICON);
    demo..window..set_title(CB_RES_TITLE);
    demo..window..center();
    demo..window..allow_highdpi();
    demo..window..master_gl();
    demo..window..disable_d3d();
    demo..window..disable_vk();
    demo..window..gl..requested..accelerated_visual = true;
    demo..window..gl..requested..double_buffering = true;
    demo..window..gl..requested..depth_bits = 24;
    demo..window..gl..requested..stencil_bits = 8;
    demo..window..gl..requested..context_enable_debug();
    demo..window..gl..requested..profile_core();
    demo..window..gl..requested..major_version = 4;
    demo..window..gl..requested..major_version = 5;
    demo..window..gl..requested..r_bits = 8;
    demo..window..gl..requested..g_bits = 8;
    demo..window..gl..requested..b_bits = 8;
    demo..window..gl..requested..a_bits = 8;
    demo..window..gl..requested..buffer_bits = 32;
    demo..window..gl..requested..accum_r_bits = 0;
    demo..window..gl..requested..accum_g_bits = 0;
    demo..window..gl..requested..accum_b_bits = 0;
    demo..window..gl..requested..accum_a_bits = 0;
    demo..window..gl..requested..stereo = false;
    demo..window..gl..requested..multisample_buffers = 0;
    demo..window..gl..requested..multisample_samples = 0;
    demo..window..gl..requested..share_with_current_context = false;
    demo..window..create();
    demo..window..framerate_hint(0);
    demo..window..framerate_limit(256);
    demo..window..termination_callback(cb_demo_clean_then_exit);
    demo..window..termination_callback_arg((void*)demo);
    demo..init_cubes();

    /* We effectively replace the engine's default task.
     * We can restore it with fe_tasknode_reset_by_label_on_window_frame(). */
    fe_tasknode onframe = fe_tasknode_by_label_on_window_frame();
    onframe..set_callback(cb_demo_onframe);
    onframe..set_callback_arg(demo);
    onframe..set_thread_count(1);
    onframe..need_gl_thread();
}

void cb_launch(size_t argc, const char * const argv[]) {
    cb_demo demo;
    demo....alloc();
    demo..init();
}
