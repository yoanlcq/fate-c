// See https://gcc.gnu.org/onlinedocs/gcc-4.8.5/gcc/X86-transactional-memory-intrinsics.html#X86-transactional-memory-intrinsics
// Compile with -mrtm.
#include <immintrin.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

void fate_transaction_begin(void) {
    unsigned status;
    if(_xtest()) // Are we already in a transaction ?
        return;
    // _XBEGIN_STARTED is actually defined as (~0u), so there is no
    // redundancy in this 'while' condition. Leave it as it is.
    do status = _xbegin(); 
    while(status != _XBEGIN_STARTED && (status & _XABORT_RETRY));
    if(status == _XBEGIN_STARTED)
        return;
    if(status & _XABORT_EXPLICIT)
        printf("Explicit abort. Code : %d\n", _XABORT_CODE(status));
    if(status & _XABORT_CONFLICT)
        printf("Memory conflict with another thread.\n");
    if(status & _XABORT_CAPACITY)
        printf("Uses too much memory.\n");
    if(status & _XABORT_DEBUG)
        printf("Debug trap.\n");
    if(status & _XABORT_NESTED)
        printf("Abort in an inner nested transaction.\n");
}

void fate_transaction_end(void) {
    unsigned status;
    if(_xtest())
        _xend();
    //else
        //fate_mt_mutex_unlock_real(mutex);
}

struct twoint {
    int a, b;
};

int display(void *data) {
    struct twoint *pair = data;
    for(;;)
        printf("%d%d\n", pair->a, pair->b);
    return 0;
}

int main(void) {
    const struct twoint ZEROES = {0, 0};
    const struct twoint ONES = {1, 1};
    struct twoint pair = ZEROES;
    /*SDL_Thread *reader =*/ SDL_CreateThread(display, "Reader", &pair);
    for(;;) {
        fate_transaction_begin();
        pair = ZEROES;
        fate_transaction_end();
        fate_transaction_begin();
        pair = ONES;
        fate_transaction_end();
    }
    return 0;
}
