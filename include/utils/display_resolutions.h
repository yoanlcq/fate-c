#ifndef DISPLAY_RESOLUTIONS_H
#define DISPLAY_RESOLUTIONS_H

#include <stdint.h>

/* #define TRUST_STEAM_2015_SURVEY */

#ifdef TRUST_STEAM_2015_SURVEY
#define ALL_DISPLAY_RESOLUTIONS_SIZE 13
#else 
#define ALL_DISPLAY_RESOLUTIONS_SIZE 50
#endif

extern const uint16_t
    ALL_DISPLAY_RESOLUTIONS[ALL_DISPLAY_RESOLUTIONS_SIZE][2];
const uint16_t *prev_resolution(uint16_t width, uint16_t height);
const uint16_t *next_resolution(uint16_t width, uint16_t height);
const uint16_t *highest_contained_resolution(uint16_t width, uint16_t height);
const uint16_t *lowest_containing_resolution(uint16_t width, uint16_t height);


#endif /* DISPLAY_RESOLUTIONS_H */
