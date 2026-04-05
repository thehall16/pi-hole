//
// Created by thehall16 on 4/5/26.
//
#include <string.h>
#include "dns_response.h"

int build_nxdomain_response(unsigned char *query, unsigned char *response, int query_len) {
    memcpy(response, query, query_len);

    response[2] = 0x81;
    response[3] = 0x83;

    response[6] = 0x00;
    response[7] = 0x00;

    response[8] = 0x00;
    response[9] = 0x00;

    response[10] = 0x00;
    response[11] = 0x00;

    return query_len;
}