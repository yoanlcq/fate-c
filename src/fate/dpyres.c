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

/*! \file fate/dpyres.c
 *  \brief Display resolutions.
 *
 * TODO
 */

#include <fate/dpyres.h>
#include <stdint.h>

const uint16_t FE_DPYRES[FE_DPYRES_LEN][2] = {
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
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
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1067,600},  /* */
    {1120,700},  /* */
    {1152,720},  /* */
    {1152,768},  /* */
    {1152,864},  /* XGA+ */
    {1176,664},
    {1280,600},
#endif
    {1280,720},  /* HD 720 */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1280,768},  /* WXGA */
#endif
    {1280,800},  /* WXGA */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1280,854},  /* */
    {1280,960},  /* */
#endif
    {1280,1024}, /* SXGA */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1344,840},  /* */
#endif
    {1360,768},  /* */
    {1366,768},  /* */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1400,1050}, /* SXGA+ */
    {1414,794},  /*  */
    {1421,800},
#endif
    {1440,900},  /* */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1440,960},  /* */
    {1440,1080}, /* */
#endif
    {1536,864},  /* */
    {1600,900},  /* */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1600,1200}, /* UXGA */
#endif
    {1680,1050}, /* WSXGA+ */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {1768,992},  /* */
#endif
    {1920,1080}, /* HD 1080 */
    {1920,1200}, /* WUXGA */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {2048,1080}, /* 2K */
    {2048,1536}, /* QXGA */
    {2560,1080}, /* UWUXGA */
#endif
    {2560,1440}, /* WQHD */
#ifndef FE_DPYRES_TRUST_STEAM_2015_SURVEY
    {2560,1600}, /* WQXGA */
    {2560,2048}, /* QSXGA */
    {3840,2160}, /* UHD-1 */
    {4096,2160} /* 4K */
#endif
};

const uint16_t *fe_dpyres_prev_in_list(uint16_t w, uint16_t h) {
    long hit = FE_DPYRES_LEN-1;
    const uint16_t *res = FE_DPYRES[hit];
    while(res[0] > w && hit > 0) res = FE_DPYRES[--hit];
    while(res[1] > h && hit > 0) res = FE_DPYRES[--hit];
    if(hit > 0)
        res = FE_DPYRES[--hit];
    return res;
}
const uint16_t *fe_dpyres_prev(uint16_t w, uint16_t h) {
    long hit = FE_DPYRES_LEN-1;
    const uint16_t *res = FE_DPYRES[hit];
    while(res[0] >= w && hit > 0) res = FE_DPYRES[--hit]; 
    while(res[1] >= h && hit > 0) res = FE_DPYRES[--hit];
    return res;
}

const uint16_t *fe_dpyres_next_in_list(uint16_t w, uint16_t h) {
    long hit = 0;
    const uint16_t *res = FE_DPYRES[hit];
    while(res[0] < w && hit < FE_DPYRES_LEN-1) res = FE_DPYRES[++hit];
    while(res[1] < h && hit < FE_DPYRES_LEN-1) res = FE_DPYRES[++hit];
    if(hit < FE_DPYRES_LEN-1)
        res = FE_DPYRES[++hit];
    return res;
}
const uint16_t *fe_dpyres_next(uint16_t w, uint16_t h) {
    long hit = 0;
    const uint16_t *res = FE_DPYRES[hit];
    while(res[0] <= w && hit < FE_DPYRES_LEN-1) res = FE_DPYRES[++hit];
    while(res[1] <= h && hit < FE_DPYRES_LEN-1) res = FE_DPYRES[++hit];
    return res;
}

