/* 
 * This file is part of F.A.T.E - the Fight And Travel game Engine.
 * Copyright (c) 2015-2016 Yoan Lecoq (yoanlecoq.io at gmail.com)
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software. 
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Contributors :
 * - Please make sure your indentation is set to 4 spaces;
 * - Please avoid exceeding 80 character columns.
 *
 */

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
