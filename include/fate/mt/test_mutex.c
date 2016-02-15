#include <fate/mt.h>


typedef struct vec2 {
    FATE_MT_MUTEX_DECL(mutex)
    float x, y;
} vec2;

void vec2_init(vec2 *v, float x, float y) {
    fate_mt_mutex_init(v->mutex);
    v->x = x;
    v->y = y;
}

void vec2_deinit(vec2 *v) {
    fate_mt_mutex_deinit(v->mutex);
}


#define NUM_ITER (1<<12)

int reader(void *arg) {
    unsigned long i;
    vec2 *v = arg;
    for(i=0 ; i<NUM_ITER ; ++i) {
        fate_mt_mutex_lock(v->mutex);
        printf("%d%d\n", (int)v->x, (int)v->y);
        fate_mt_mutex_unlock(v->mutex);
    }
    return 0;
}

int writer(void *arg) {
    unsigned long i;
    vec2 *v = arg;
    for(i=0 ; i<NUM_ITER ; ++i) {
        fate_mt_mutex_lock(v->mutex);
        v->x = v->y = 0;
        fate_mt_mutex_unlock(v->mutex);
        fate_mt_mutex_lock(v->mutex);
        v->x = v->y = 1;
        fate_mt_mutex_unlock(v->mutex);
    }
    return 0;
}

int main(void) {
    vec2 v;
    vec2_init(&v, 0, 0);
    SDL_Thread *thread = SDL_CreateThread(writer, "Writer", &v);
    reader(&v);
    SDL_WaitThread(thread, NULL);
    vec2_deinit(&v, 0, 0);
    return 0;
}
