/* Designing entities : */
/* (There's no need for a multiverse here) */
struct cb_demo {
    fe_window window;
    fe_viewport viewport;
    //fe_region region; There's one by default.
    cb_camera camera;
    cb_cubedata cubedata;
    cb_cube cube;
};
struct cb_camera {
    fe_view3d view;
    fe_wunit distance;
    fe_wquat rotation;
}
struct cb_cubedata {
    fe_meshdata3d mesh_data;
    fe_material material;
};
struct cb_cube {
    fe_mesh3d mesh;
};

void cb_camera_init(fe_idx cam) {
    cb_camera___alloc_members(cam);

    fe_view3d view = cb_camera__view(cam);
    fe_view3d_reset(view);
    fe_tf3d tf = fe_view3d__tf3d(view) = fe_tf3d___alloc();

    cb_camera__distance(cam) = 1000;
    fe_wquat_zero(cb_camera__rotation(cam));
    fe_wmat4_lookat(
        fe_tf3d__wmat4(tf), 
        (fe_wvec3){0,0,0}, 
        (fe_wvec3){0,1,0}
    );
    /* camera..tf..wmat4..lookat(.....) 
     * fe_wmat4_lookat(fe_tf3d_wmat4(cb_camera_tf(camera)));
     */
}

void cb_cubedata_init(size_t cubedata) {
    cb_cubedata___alloc_members();
    fe_meshdata3d_load_from_cube(cb_cubedata__meshdata3d(cubedata));
    fe_material_fill_red(cb_cubedata__material(cubedata));
}

void cb_cube_init(size_t cube, size_t cubedata) {
    cb_cube___alloc_members(cube);
    fe_mesh3d__meshdata3d(cb_cube__mesh(cube)) = cb_cubedata__meshdata(cubedata);
    fe_tf3d_reset(cb_cube__tf(cube));
    //C++ :
    //cube.mesh(cubedata.meshdata())
    //Slayed C :
    //cube....alloc_members();
    //cube..mesh..meshdata = cubedata..meshdata;
    //cube..tf..reset();
}

void cb_demos_update(void *data) {
    /* update all demos, call deinit() if we're quitting, then close. */
    cb_demo demo = cb_demo___start();
    if(fe_window_closing(cb_demo__window(demo))) {
        cb_demo_deinit(demo);
        exit(EXIT_SUCCESS);
    }
    cb_camera cam = cb_demo__camera(demo);
    cb_camera__distance(cam) += fe_mouse->wheel_delta.y;
    if(fe_mouse->left.pressed) {
        angle_x += fe_mouse->move.x;
        angle_y += fe_mouse->move.y;
    }
}
void cb_demo_deinit(size_t demo) {
    cb_camera_deinit(cb_demo__camera(demo));
    cb_cubedata_deinit(cb_demo__cubedata(demo));
    cb_cube_deinit(cb_demo__cube(demo));
    cb_demo___dealloc_members(demo);
}

void cb_demo_init(size_t demo) {
    cb_demo___alloc_members(demo);
    cb_demo___zero_members(demo);

    cb_camera cam = cb_demo__camera(demo);
    cb_camera_init(cam);
    cb_cubedata_init(cb_demo__cubedata(demo));
    cb_cube_init(cb_demo__cube(demo));
    fe_view3d view = cb_camera__view(cam);

    fe_window win = cb_demo__window(demo);
    fe_window_reset(win);
    fe_window_set_size(win, (fe_uvec2){800, 600});
    fe_window_set_icon(win, CB_RES_ICON);
    fe_window_set_title(win, CB_RES_TITLE);
    fe_window_center(win);
    fe_window_enable_gl(win);

    fe_viewport vp = cb_demo__viewport(vp);
    fe_viewport_set_window(vp, win);
    fe_viewport_set_insets(vp, (fe_vec4){0.f, 0.f, 0.f, 0.f});
    fe_viewport_set_view(vp, view);

    fe_tasknode tsk = fe_tasknode_retrieve_tick();
    fe_tasknode mytask = fe_tasknode___alloc();
    fe_tasknode_set_thread_count(mytask, 1);
    fe_tasknode_set_func(mytask, cb_demos_update);
    fe_tasknode_depend(mytask, tsk);
}

void cb_main(size_t argc, const char * const argv[]) {
    cb_demo_init(cb_demo___alloc());
}
