double target_ms_per_frame = 1000.0/60.0;
double frame_accumulator = 0.0;
uint64_t frameno = 0;
sfClock *clock = sfClock_create();
sfInt64 current_time, last_time = clock.microseconds;

void next_frame(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* See http://www.opengl-tutorial.org/miscellaneous/an-fps-counter/ */
    current_time = sfClock_getElapsedTime(clock).microseconds;
    ++frameno, ++frame_accumulator;
    if(current_time - last_time >= 100000LL)
    {
        fst_set_f64("window.ms_per_frame", 100.0/frame_accumulator);
        frame_accumulator = 0.0;
        last_time += 100000LL;
    }

    world_time += time_taken;
    compute_or_get_current_tick();
    compute_or_get_next_tick();
    render(interpolated_world);
    sfWindow_display(window);
}
