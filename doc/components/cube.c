void cb_camera_init(size_t ths) {
    cb_camera cam = ths;
    cam....alloc_members();

    cam..view..reset();
    cam..view..transform <= ....alloc();
    cam..view..transform..rotation <= ....alloc(1);
    cam..view..transform..rotation..reset();
    cam..view..transform..translation <= ....alloc(1);
    cam..view..transform..translation..wvec3..fill(0,0,-1000);

    cam..vel..derivate(cam..view..transform..translation);
    cam..angular_vel..derivate(cam..view..transform..rotation);
}

void cb_cubedata_init(size_t cubedata) {
    cubedata....alloc_members();
    cubedata..meshdata..load_from_cube();
    cubedata..material..fill_red();
}

void cb_cube_init(size_t cube, size_t cubedata) {
    cube....alloc_members();
    cube..mesh..meshdata = cubedata..meshdata;
    cube..transform..reset();
}

void cb_demos_update(void *data) {
    /* update all demos, call deinit() if we're quitting, then close. */

    cb_demo demo = ....start();
    if(demo..window..closing) {
        demo..deinit();
        demo....dealloc();
        exit(EXIT_SUCCESS);
    }
    demo..camera..vel = 10*fe_mouse->wheel_delta.y;
    if(fe_mouse->left.pressed)
        demo..camera..angular_vel = fe_mouse->move;
}
void cb_demo_deinit(size_t demo) {
    demo..camera..deinit();
    demo..cube..deinit();
    demo..cubedata..deinit();
    demo....dealloc_members();
}

void cb_demo_init(size_t demo) {
    demo....alloc_members();
    demo....zero_members();

    demo..camera..init();
    demo..cubedata..init();
    demo..cube..init(demo..cubedata);

    demo..window..reset();
    demo..window..size..fill(800, 600);
    demo..window..icon = CB_RES_ICON;
    demo..window..title = CB_RES_TITLE;
    demo..window..center();
    demo..window..enable_gl();

    demo..viewport..window = demo..window;
    demo..viewport..insets..fill(0.f, 0.f, 0.f, 0.f);
    demo..viewport..view = demo..camera..view;

    fe_tasknode mytask, tsk = ..get_game_update();
    mytask = ....alloc();
    mytask..thread_count = 1;
    mytask..callback = cb_demos_update;
    mytask..depend_on(tsk);
}

void cb_launch(size_t argc, const char * const argv[]) {
    cb_demo demo;
    demo....alloc();
    demo..init();
}
