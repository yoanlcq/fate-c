#include <fate/wss13.h>
#include <fate/tcp6.h>

/*

status fe_tcp6_wss_handshake(fe_tcp6 *s, const fe_wss_requirements *s, 
                             const char **chosen_protocol, 
                             const void *data, size_t len);
status fe_tcp6_wss_parse_dataframe(fe_tcp6 *s, fe_wss_dataframe *df, const void *data, size_t len);
const char* fe_wss_dataframe_get_payload(const fe_wss_dataframe *df, size_t *out_len);
void fe_tcp6_wss_request_close(fe_tcp6 *s, const char *reason);


char buf[4096];
sock = accept();
recv(sock, buf, sizeof(buf));
if(strncmp(buf, "GET", 3))
    send 400 bad request;
ptr = strchr(buf '\r')-3;
if(ptr < "1.1")
    send 400 bad request;
look_origin_header();
if(!origin || origin is spam)
    send 403 forbidden;
beware if coming from same ipaddress (denial of service).
str = "Sec-WebSocket-Protocol";
look_at_the_protocols // chat, superchat
str = "Sec-WebSocket-Version: ";
ws_version = strtoul(strstr(buf, str)+strlen(str), NULL, 10);
if(ws_version not supported)
    send 400 bad-request with Sec-WebSocket-Version supported version;
str = "Sec-WebSocket-Key: ";
strcpy(key, strstr(buf, str));
*strchr(key, '\r') = '\0';
strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
ws_accept = base64(sha1(key));

send(
"HTTP/1.1 101 Switching Protocols\r\n"
"Upgrade: websocket\r\n"
"Connection: Upgrade\r\n"
"Sec-WebSocket-Accept: " ws_accept "=\r\n"
"\r\n"
);
//handshake complete!
remember_client();

struct ws_data_frame {
    unsigned fin                     : 1;
    unsigned rsv1                    : 1;
    unsigned rsv2                    : 1;
    unsigned rsv3                    : 1;
    unsigned opcode                  : 4;
    unsigned masked                  : 1;
    unsigned payload_length          : 7;
    uint64_t extended_payload_length : 64;
    uint32_t masking_key             : 32;
    // uint8_t  payload_data            : 8;
};
if(!df->masked)
    close();
uint64_t getlength(ws_data_frame *df) {
    switch(df->payload_length) {
    case 126: return *(uint16_t*)&df->extended_payload_length;
    case 127: return *(uint64_t*)&df->extended_payload_length;
    }
    return df->payload_length;
}
void decode_payload(ws_data_frame *df, uint64_t len) {
    size_t i;
    char *decoded, *encoded;
    for(i=0; i<len; ++i)
        decoded[i] = encoded[i] ^ ((const uint8_t*)&df->masking_key)[i%4];
}
const char *get_payload_ptr(ws_data_frame *df) {
    return &df->payload_data;
}
while(receiving) {
    switch(df->opcode) {
    case 0x0: continuation;
    case 0x1: utf-8;
    case 0x2: binary_data;
    case 0x8: 
        if(is_reply_to_sent_close_frame)
            close(socket);
        else
            send_close_frame();
    case 0x9: ping;
        df->opcode = 0xA;
        send(df); //send pong.
    }
    if(df->fin == 1)
        process_and_reply();
    else {
        accumulate_data();
    }
    if(want_to_close)
        send_close_frame();
}
void send_close_frame() {
    df->opcode = 0x8;
    send(df);
    //Don't send anymore !
}
*/
