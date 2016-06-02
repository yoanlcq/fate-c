/* 
 * Rules : 
 * '..' is the user-specified method reach operator.
 * '....' is the generated method reach operator.
 * Methods guess whether they must take the 'this' 1st parameter by value
 * or by address.
 */

typedef unsigned cb_camera;
typedef unsigned cb_cubedata;
typedef unsigned cb_cube;
typedef unsigned cb_demo;


void cb_camera_init(cb_camera ths) {
    cam....alloc_members();

    cam..view..reset();
    cam..view..transform....alloc();
    cam..view..transform..rotation....alloc();
    cam..view..transform..rotation..reset();
    cam..view..transform..translation....alloc();
    cam..view..transform..translation..wvec3..fill(0,0,-1000);

    cam..vel..derivate(cam..view..transform..translation);
    cam..angular_vel..derivate(cam..view..transform..rotation);
}

void cb_cubedata_init(cb_cubedata cubedata) {
    cubedata....alloc_members();
    cubedata..meshdata..load_from_cube();
    cubedata..material..fill_red();
}

void cb_cube_init(cb_cube cube, cb_cubedata cubedata) {
    cube....alloc_members();
    cube..mesh..meshdata = cubedata..meshdata;
    cube..transform..reset();
}

void cb_demos_update(fe_taskctx *ctx) {
    /* update all demos, call deinit() if we're quitting, then close. */

    cb_demo demo;
    demo....start();
    if(demo..window..closing) {
        demo..deinit();
        demo....dealloc();
        exit(EXIT_SUCCESS);
    }
    demo..camera..vel = 10*fe_mouse->wheel_delta.y;
    if(fe_mouse->left.pressed)
        demo..camera..angular_vel = fe_mouse->move;
}
void cb_demo_deinit(cb_demo demo) {
    demo..camera..deinit();
    demo..cube..deinit();
    demo..cubedata..deinit();
    demo....dealloc_members();
}

void cb_demo_init(cb_demo demo) {
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

    fe_tasknode mytask, tsk;
    tsk..retrieve_game_update();
    mytask....alloc();
    mytask..
    mytask..enable_work_stealing();
    mytask..set_callback(cb_demo_update);
    mytask..depend_on(tsk);
}

void cb_launch(size_t argc, const char * const argv[]) {
    cb_demo demo;
    demo....alloc();
    demo..init();
}



/* UNRELATED */

void cb_cubes_update(fe_taskctx *ctx) {
    /* Fill tmp scratchpad using various inputs :
     * - User, AI input;
     * - Own data;
     * - Any number of other entities' data; 
     *
     * Then write to own data using own tmp scratchpad.
     */
    cb_cube cube;
    cube....parallel_foreach(ctx)
        (cube..react_to_user_input_funcptr)(cube);
    cube....parallel_foreach(ctx)
        (cube..react_to_other_cubes_funcptr)(cube);
    cube....parallel_foreach(ctx)
        (cube..commit_tmp_scratchpad_funcptr)(cube);
}
