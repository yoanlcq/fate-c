#ifndef FATE_DPYRES_H
#define FATE_DPYRES_H

#include <stdint.h>

#define FATE_DPYRES_TRUST_STEAM_2015_SURVEY

#ifdef FATE_DPYRES_TRUST_STEAM_2015_SURVEY
#define FATE_DPYRES_LEN 13
#else 
#define FATE_DPYRES_LEN 50
#endif

extern const uint16_t FATE_DPYRES[FATE_DPYRES_LEN][2];
const uint16_t *fate_dpyres_prev_in_list(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_next_in_list(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_prev(uint16_t width, uint16_t height);
const uint16_t *fate_dpyres_next(uint16_t width, uint16_t height);


#endif /* FATE_DPYRES_H */
