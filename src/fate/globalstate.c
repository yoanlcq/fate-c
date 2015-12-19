#include <stdlib.h>
#include <stdio.h>
#include <fate/sys.h>
#include <fate/log.h>
#include <fate/globalstate.h>

fate_globalstate fate_gs_;
fate_globalstate *fate_gs = &fate_gs_;

void fate_globalstate_init(fate_globalstate *gs) {

    fate_logf_err = fate_logf_err_to_console;

    char *game_path = fate_sys_getgamepath();
    if(!game_path)
        fate_fatal("Couldn't find the game's path.\nSorry.\n");
    fate_sys_set_current_directory(game_path);
    free(game_path);

#define HELPER(X, P) \
    gs->log##X##_stream = fopen(P, "w"); \
    if(!gs->log##X##_stream) \
        fate_fatal("Could not open \"%s\" for writing.\n", P); \
    fate_log##X = fate_log##X##_to_console_and_stream;
    HELPER(f_err,   "data/logs/err.log");
    HELPER(f,       "data/logs/out.log");
    HELPER(f_trace, "data/logs/trace.log");
    HELPER(f_video, "data/logs/video.log");
    HELPER(f_audio, "data/logs/audio.log");
#undef HELPER
    fate_sys_crash_handler_setup();
}
void fate_globalstate_deinit(fate_globalstate *gs) {
#define HELPER(X) if(X!=stdout && X!=stderr) fclose(X);
    HELPER(gs->logf_stream);
    HELPER(gs->logf_trace_stream);
    HELPER(gs->logf_err_stream);
    HELPER(gs->logf_video_stream);
    HELPER(gs->logf_audio_stream);
#undef HELPER
}
