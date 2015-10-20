#include <utils/display_resolutions.h>
#include <stdint.h>
/*
 * A list of (theoretically) all the possible display resolutions, 
 * ordered by width, then height.
 * Taken from https://upload.wikimedia.org/wikipedia/commons/0/0c/Vector_Video_Standards8.svg 
 * and Steam's 2015 Hardware Survey.
 * Last updated on July 2015.
 */

const uint16_t ALL_DISPLAY_RESOLUTIONS[ALL_DISPLAY_RESOLUTIONS_SIZE][2] = {
#ifndef TRUST_STEAM_2015_SURVEY
    {320,200},   /* CGA */
    {320,240},   /* QVGA */
    {352,288},   /* CIF */
    {384,288},   /* SIF */
    {480,320},   /* HVGA */
    {640,480},   /* VGA */
    {768,576},   /* PAL */
    {800,480},   /* WVGA */
    {800,600},   /* SVGA */
    {854,480},   /* WVGA */
    {1024,576},  /* PAL */
    {1024,600},  /* WSVGA */
#endif
    {1024,768},  /* XGA */
#ifndef TRUST_STEAM_2015_SURVEY
    {1067,600},  /* */
    {1120,700},  /* */
    {1152,720},  /* */
    {1152,768},  /* */
    {1152,864},  /* XGA+ */
    {1176,664},
    {1280,600},
#endif
    {1280,720},  /* HD 720 */
#ifndef TRUST_STEAM_2015_SURVEY
    {1280,768},  /* WXGA */
#endif
    {1280,800},  /* WXGA */
#ifndef TRUST_STEAM_2015_SURVEY
    {1280,854},  /* */
    {1280,960},  /* */
#endif
    {1280,1024}, /* SXGA */
#ifndef TRUST_STEAM_2015_SURVEY
    {1344,840},  /* */
#endif
    {1360,768},  /* */
    {1366,768},  /* */
#ifndef TRUST_STEAM_2015_SURVEY
    {1400,1050}, /* SXGA+ */
    {1414,794},  /*  */
    {1421,800},
#endif
    {1440,900},  /* */
#ifndef TRUST_STEAM_2015_SURVEY
    {1440,960},  /* */
    {1440,1080}, /* */
#endif
    {1536,864},  /* */
    {1600,900},  /* */
#ifndef TRUST_STEAM_2015_SURVEY
    {1600,1200}, /* UXGA */
#endif
    {1680,1050}, /* WSXGA+ */
#ifndef TRUST_STEAM_2015_SURVEY
    {1768,992},  /* */
#endif
    {1920,1080}, /* HD 1080 */
    {1920,1200}, /* WUXGA */
#ifndef TRUST_STEAM_2015_SURVEY
    {2048,1080}, /* 2K */
    {2048,1536}, /* QXGA */
    {2560,1080}, /* UWUXGA */
#endif
    {2560,1440}, /* WQHD */
#ifndef TRUST_STEAM_2015_SURVEY
    {2560,1600}, /* WQXGA */
    {2560,2048}, /* QSXGA */
    {3840,2160}, /* UHD-1 */
    {4096,2160} /* 4K */
#endif
};

#define ADRA ALL_DISPLAY_RESOLUTIONS
#define ADRS ALL_DISPLAY_RESOLUTIONS_SIZE
#define crs cur_resolution
const uint16_t *prev_resolution(uint16_t w, uint16_t h) {
    int hit = ADRS-1;
    const uint16_t *res = ADRA[hit];
    while(res[0] > w && hit > 0) res = ADRA[--hit];
    while(res[1] > h && hit > 0) res = ADRA[--hit];
    if(hit > 0)
        res = ADRA[--hit];
    return res;
}
const uint16_t *highest_contained_resolution(uint16_t w, uint16_t h) {
    int hit = ADRS-1;
    const uint16_t *res = ADRA[hit];
    while(res[0] >= w && hit > 0) res = ADRA[--hit]; 
    while(res[1] >= h && hit > 0) res = ADRA[--hit];
    return res;
}

const uint16_t *next_resolution(uint16_t w, uint16_t h) {
    int hit = 0;
    const uint16_t *res = ADRA[hit];
    while(res[0] < w && hit < ADRS-1) res = ADRA[++hit];
    while(res[1] < h && hit < ADRS-1) res = ADRA[++hit];
    if(hit < ADRS-1)
        res = ADRA[++hit];
    return res;
}
const uint16_t *lowest_containing_resolution(uint16_t w, uint16_t h) {
    int hit = 0;
    const uint16_t *res = ADRA[hit];
    while(res[0] <= w && hit < ADRS-1) res = ADRA[++hit];
    while(res[1] <= h && hit < ADRS-1) res = ADRA[++hit];
    return res;
}

#undef ADRA
#undef ADRS
#undef crs

