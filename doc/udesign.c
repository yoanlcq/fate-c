/* When designing the Cyberzard entity, I specified what follows :
 * - I'm using my game's namespace ('cj');
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
#include <stdint.h>
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
/* #include <cj/cbzard.priv.h> */
typedef uint16_t cj_cbzard;
size_t cj_cbzard_chunkfunc(size_t n);
void cj_cbzard___allocv(cj_cbzard array[], cj_cbzard count);
cj_cbzard cj_cbzard___alloc(void);
void cj_cbzard___deallocv(cj_cbzard array[], cj_cbzard count);
void cj_cbzard___dealloc(cj_cbzard self);
#define cj_cbzard__x(_id) (CJ_CBZARD_S_I.pod_0[_id])
#define cj_cbzard__y(_id) (CJ_CBZARD_S_I.pod_1[_id])
#define cj_cbzard__z(_id) (CJ_CBZARD_S_I.pod_2[_id])
#define cj_cbzard__life(_id)  (CJ_CBZARD_S_I.pod_3[_id].pod_0[0])
#define cj_cbzard__power(_id) (CJ_CBZARD_S_I.pod_3[_id].pod_0[1])
#define cj_cbzard__mana(_id)  (CJ_CBZARD_S_I.pod_3[_id].pod_1)

/* cj_cbzard.h (Generated from cj/cbzard.c) */
/* #include <fate.h> */
/* #include <cj/cbzard.api.h> */
#if FE_STATIC_BUILD /* Everything is compiled together. */
void cj_cbzard_mysharedfunc(cj_cbzard self);
#elif FE_DYNAMIC_BUILD /* The game is built to be hot-reloadable. */
extern void (*cj_cbzard_mysharedfunc)(cj_cbzard self);
#elif FE_DLL_BUILD /* We are a DLL to be used by the hot-reloadable build */
FE_EXPORT void cj_cbzard_mysharedfunc(cj_cbzard self);
#endif
#if FE_DYNAMIC_BUILD
void cj_cbzard_hotreload(fe_hr_dll dll); /* Registered for hot-reloading */
#endif


/* cj_cbzard.reloadable.c (only if FE_DYNAMIC_BUILD) */
void (*cj_cbzard_mysharedfunc)(cj_cbzard self);
void cj_cbzard_hotreload(fe_hr_dll dll) {
    cj_cbzard_mysharedfunc = fe_hr_getsym(dll, "cj_cbzard_mysharedfunc");
}


/* cj_cbzard.c (Hand-written) */
/* #include <cj/cbzard.h> */
size_t cj_cbzard_chunkfunc(size_t n) {
    /* 32 Cyberzards at first, increase by 16 as needed */
    return n ? 16 : 32;
}
static void cj_cbzard_kill(cj_cbzard self) {
    cj_cbzard__life(self) = 0;
}
void cj_cbzard_mysharedfunc(cj_cbzard self) {
    cj_cbzard_kill(self);
}


/* cj_boss.c (Hand-written) */
/* #include <cj/boss.h> */
/* #include <cj/cbzard.h> */
/* Our DLL here is linked against the cbzard DLL. */
void cj_boss_killcbzard(cj_boss self, cj_cbzard target) {
    cj_cbzard_mysharedfunc(target);
}

int main(int argc, char *argv[]) {
    cj_cbzard_kill(0);
    return 0;
}



/* Here is the directory layout of the FATE SDK, as downloaded :
 *
 * FATE_<version>_<dev_platform>/
 * `- bin/
 *    `- fate.exe (The editor)
 *    `- ld_deps
 *    `- (Other tools here)
 * `- (Other data here. shared libs ?)
 *
 * FATE_build/ (specified by user)
 * `- (all .o files for FATE here)
 *
 * multiverses_build/ (specified by user)
 * `- Cybernetic_Jurassic/
 *    `- (all .o files for CJ here)
 *
 * multiverses/ (specified by user)
 * `- Grisui/
 * `- Cybernetic_Jurassic/
 *    `- fate/
 *       `- settings.cfg (Picked-up by the engine)
 *       `- (All files required to display enity schemas to the user, etc.)
 *    `- src/
 *       `- cj/
 *          `- cbzard.c
 *    `- gen/ (Read-only ! The engine rebuilds its content as needed.)
 *       `- cj/
 *          `- cbzard.h
 *    `- art/
 *       `- (This is the artist's corner. Put content here as you wish,
 *           but you should keep it tidy by making subdirectories.
 *           Basically any content you put here is picked up on-the-fly 
 *           by the engine - baked data is created from it, placed in your
 *           game's asset directory, and immediately usable from your
 *           code.
 *
 *           The engine reloads data on-the-fly as the locale
 *           is changed within the game.
 *
 *           What problems must this solve ?
 *           - It should be fast to add l10n late to the project.
 *           - Changing the locale should reload the appropriate version
 *             of the necessary entities.
 *           - It should still be left to the user to decide how l10n takes
 *             place.
 *           Solution :
 *           fe_load_l10n("%/hero.%.obj");
 *           If desired, write a macro :
 *           #define cj_load(str) fe_load_l10n("%/" str)
 *
 *           Example content :
 *             Fonts, images, 3D models, musics, sounds, text files).
 *    `- res/
 *       `- (This folder is where mandatory resources are placed - they
 *           are well-known, required, localized, and separated from regular
 *           assets.
 *           Example content :
 *             Copyright info, company name, icons for Android, 
 *             Splash Screen for iOS, and more).
 *
 * dist/ (specified by user)
 * `- Cybernetic_Jurassic_<version>.deb
 * `- Cybernetic_Jurassic_<version>_<platform>_dynamic/
 * `- Cybernetic_Jurassic_<version>_<platform>/
 *    `- (Binaries, DLLs, and baked resources here.
 *        The user can choose whether they should be jammed together
 *        or arranged within directories. The engine can be instructed
 *        to includes sources.);
 *
 * FATE utilities :
 *  fate-setup-android
 *  fate-setup-ios
 *  fate-setup-windows
 *  fate-setup-linux
 *  fate-setup-osx
 *  fate-setup-emscripten
 *  fate-create-multiverse # Create a template multiverse
 *  fate-build-android
 *  fate-build-ios
 *  fate-build-windows32-msvc
 *  fate-build-windows64-msvc
 *  fate-build-windows32-gcc
 *  fate-build-windows64-gcc
 *  fate-build-windows32-clang
 *  fate-build-windows64-clang
 *  fate-build-linux32-gcc
 *  fate-build-linux64-gcc
 *  fate-build-linux32-clang
 *  fate-build-linux64-clang
 *  fate-build-osx-gcc
 *  fate-build-osx-clang
 *  fate-build-emscripten
 *  fate-dist <recipe>
 *
 * Example settings.cfg:
 *  company_name = Foobar
 *  description = A Cube spinning around.
 *  legal_copyright = Foobar
 *  legal_trademarks = ...
 *  name = Cube Demo
 *  name_ident = cube_demo
 *  executable_name = cube_demo
 *  version = 1.0
 *  domain = org.author.cube
 *  android_activity = CubeActivity
 *
 *  [recipe win32-portable]
 *  target = windows32-msvc
 *  include_sources = no
 *  portable = yes
 *
 */
