#include <stdio.h>
#include <fate/sys.h>
#include <fate/log.h>
#include <fate/globalstate.h>

fate_globalstate fate_gs_;
fate_globalstate *fate_gs = &fate_gs_;

void fate_globalstate_init(fate_globalstate *gs) {
    gs->logf_stream = stdout;
    fate_logf = fate_logf_to_console;
    gs->logf_trace_stream = stderr;
    fate_logf_trace = fate_logf_trace_to_console;
    gs->logf_err_stream = stderr;
    fate_logf_err = fate_logf_err_to_console;
    gs->logf_video_stream = stdout;
    fate_logf_video = fate_logf_video_to_console;
    gs->logf_audio_stream = stdout;
    fate_logf_audio = fate_logf_audio_to_console;
    fate_sys_crash_handler_setup();
}
void fate_globalstate_deinit(fate_globalstate *gs) {

}
