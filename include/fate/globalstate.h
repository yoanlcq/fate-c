#ifndef FATE_GLOBALSTATE_H
#define FATE_GLOBALSTATE_H

struct fate_globalstate {
    FILE *logf_stream;
    FILE *logf_trace_stream;
    FILE *logf_err_stream;
    FILE *logf_video_stream;
    FILE *logf_audio_stream;
    /* fate_vfs vfs; */
};
typedef struct fate_globalstate fate_globalstate;

extern fate_globalstate  fate_gs_;
extern fate_globalstate *fate_gs;

void fate_globalstate_init(fate_globalstate *gs);
void fate_globalstate_deinit(fate_globalstate *gs);


#endif /* FATE_GLOBALSTATE_H */
