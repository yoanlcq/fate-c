#include <stdint.h>

#define FE_RELOADABLE

/* When designing the Cyberzard entity, I specified what follows :
 * - I'm using my game's namespace ('cj_');
 * - The index type name is 'cbzard', it uses 16 bits;
 * - For some reason, a cyberzard contains 6 integers : 
 *   32 bits : x, y, z;
 *   16 bits : life, power;
 *    8 bits : mana;
 * - Because I know the data access patterns, I want to pack 'life', 'power'
 *   and 'mana' together. I leave it up to the engine to pack them in a 
 *   memory-efficient way. I, as the user, don't have to know about this because
 *   I use accessors.
 * - Because I know the allocation patterns, I say that the function 
 *   returning the number of Cyberzards to pre-allocate depending on the
 *   pre-allocation's index is named "cj_cbzard_chunkfunc".
 */

/* cj_cbzard.priv.h (generated) */
#define CJ_CBZARD_TYPEID 0x5245 /* The user can see it */
#define CJ_CBZARD_S cj_0x5245_S /* S stands for Structure */
#define CJ_CBZARD_S_I cj_0x5245 /* S_I stands for Structure Instance */
/* All the fields have crappy names so the user isn't tempted to acces them */
#define MAX_CBZARDS 10 /* For instance */
struct CJ_CBZARD_S {
    uint32_t pod_0[MAX_CBZARDS];
    uint32_t pod_1[MAX_CBZARDS];
    uint32_t pod_2[MAX_CBZARDS];
    struct {
        uint16_t pod_0[2];
        uint8_t  pod_1;
    } pod_3[MAX_CBZARDS];
};
struct CJ_CBZARD_S CJ_CBZARD_S_I;


/* cj_cbzard.api.h (generated) */
/* #include "cj_cbzard.priv.h" */
typedef uint16_t cj_cbzard;
size_t cj_cbzard_chunkfunc(size_t n);
#define cj_cbzard__x(_id) (CJ_CBZARD_S_I.pod_0[_id])
#define cj_cbzard__y(_id) (CJ_CBZARD_S_I.pod_1[_id])
#define cj_cbzard__z(_id) (CJ_CBZARD_S_I.pod_2[_id])
#define cj_cbzard__life(_id)  (CJ_CBZARD_S_I.pod_3[_id].pod_0[0])
#define cj_cbzard__power(_id) (CJ_CBZARD_S_I.pod_3[_id].pod_0[1])
#define cj_cbzard__mana(_id)  (CJ_CBZARD_S_I.pod_3[_id].pod_1)


/* cj_cbzard.h (Hand-written) */
/* #include <cj_cbzard.api.h> */
void cj_cbzard_mysharedfunc(cj_cbzard self);


/* cj_cbzard.c (Hand-written) */
/* #include "cj_cbzard.h" */
FE_RELOADABLE size_t cj_cbzard_chunkfunc(size_t n) {
    /* 32 Cyberzards at first, increase by 16 as needed */
    return n ? 16 : 32;
}
static void cj_cbzard_kill(cj_cbzard self) {
    cj_cbzard__life(self) = 0;
}
FE_RELOADABLE void cj_cbzard_mysharedfunc(cj_cbzard self) {
    cj_cbzard_kill(self);
}

int main(int argc, char *argv[]) {
    cj_cbzard_kill(0);
    return 0;
}
