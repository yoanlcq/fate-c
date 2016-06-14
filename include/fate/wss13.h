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

/*! \file fate/wss13.h
 *  \brief The WebSocket 13 server implementation.
 *  \defgroup wss13 WebSocket 13
 *
 * @{
 */


#ifndef FE_WSS13_H
#define FE_WSS13_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*! \brief TODO*/
struct fe_wss13_requirements {
    uint32_t websockets_version; //13 normally.
    uint8_t max_conns_from_same_ipadress;
    bool accept_empty_origin;
    const char **blacklisted_origins;
    size_t blacklisted_origin_count;
    const char **accepted_uris;
    size_t accepted_uri_count;
    const char **supported_protocols;
    size_t supported_protocol_count;
};
/*! \brief TODO*/
typedef struct fe_wss13_requirements fe_wss13_requirements;

/*! \brief TODO*/
enum fe_wss13_handshake_status {
    FE_WSS13_HS_OK = 0,
    FE_WSS13_HS_BAD_VERSION,
    FE_WSS13_HS_SAME_IP_LIMIT,
    FE_WSS13_HS_BLACKLISTED,
    FE_WSS13_HS_NO_ORIGIN,
    FE_WSS13_HS_UNSUPPORTED_PROTO,
    FE_WSS13_HS_BAD_HEADER,
    FE_WSS13_HS_BAD_URI
};
/*! \brief TODO*/
typedef enum fe_wss13_handshake_status fe_wss13_handshake_status;

/*! \brief TODO*/
enum fe_wss13_status {
    FE_WSS13_UTF8 = 0,
    FE_WSS13_BIN,
    FE_WSS13_CLOSING,
    FE_WSS13_MORE_PACKETS,
    FE_WSS13_BYTES_LEFT,
    FE_WSS13_PING,
    FE_WSS13_PONG
};
/*! \brief TODO*/
typedef enum fe_wss13_status fe_wss13_status;

/*! \brief TODO*/
struct fe_wss13_handshake {
    fe_wss13_handshake_status status;
    const char *request_uri;
    const char *protocol;
    uint32_t websockets_version;
};
/*! \brief TODO*/
typedef struct fe_wss13_handshake fe_wss13_handshake;

/* Typical code (newer):
 *
 * fe_tcp6 sock;
 * //Assume sock is an accepted connection to a new peer.
 * char buf[sizeof(fe_wss13_dataframe) + 4096];
 * ssize_t recvd = fe_tcp6_recv(&sock, buf, sizeof(buf));
 * //Assume you didn't miss any bytes.
 *
 * fe_wss13_requirements req = {0};
 * fe_wss13_handshake hs;
 * fe_wss13_handshake_parse(&hs, buf, recvd, &req);
 * //You now have useful info in 'hs'.
 * if(hs->status == FE_WSS13_HS_BAD_HEADER) {
 *     //It might be a regular protocol. Handle it.
 *     return;
 * }
 * fe_wss13_handshake_reply(&sock, &hs));
 * if(hs->status)
 *     return;
 * fe_wss13 wss;
 * fe_wss13_init(&wss, &sock);
 * // From now on, forget about 'sock'.
 * fe_wss13_status status;
 * //A call to recv() clears the internal buffer if the previous
 * //status was not FE_WSS13_MORE.
 * //That buffer grows automatically as needed.
 * recvds = fe_wss13_recv(&wss, 4096, &status);
 * switch(status) {
 * case FE_WSS13_MORE_PACKETS:
 *     // Call recv() again sometime later!
 *     // Then access your payloads using :
 *     // fe_wss13_get_payload_count();
 *     // fe_wss13_get_payload_at();
 *     break;
 * case FE_WSS13_BYTES_LEFT:
 *     // Call recv() again soon !
 *     // You still have bytes to recv for the latest payload.
 *     break;
 * case FE_WSS13_UTF8: 
 *     const char *utf8 = fe_wss13_get_payload_at(&wss, 0);
 *     fe_wss13_reply(&wss, utf8, strlen(utf8));
 *     break;
 * case FE_WSS13_BIN: parse_bin(fe_wss13_get_payload(&wss)); break;
 * case FE_WSS13_CLOSING: fe_wss13_close(&wss, "OK, got it."); return;
 * case FE_WSS13_PING: fe_wss13_pong(&wss); break;
 * case FE_WSS13_PONG:
 *     bool fair = fe_wss13_is_pong_valid(&wss, buf, recvd);
 *     if(!fair)
 *         ; //What do you want to do ?
 *     break;
 * }
 *
 * //If we want to send a ping :
 * fe_wss13_ping(&wss, data, len);
 * //If we want to close the communication :
 * fe_wss13_close(buf, "I don't wanna talk to you anymore.");
 *
 */

/*! @} */
#endif /* FE_WSS13_H */
