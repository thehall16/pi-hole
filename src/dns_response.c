//
// Created by thehall16 on 4/5/26.
//
#include <string.h>
#include "dns_response.h"

int build_nxdomain_response(unsigned char *query, unsigned char *response, int query_len) {
    memcpy(response, query, query_len);

    // Set packet bits to read as response QR -> 1
    // Recursion available RA -> 1
    // RCODE: 0011 -> NXDOMAIN
    // Bit #: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
    //        QR OPCODE      AA TC RD RA Z   RCODE

    // 1000 0001
    response[2] = 0x81;
    // 1000 0011
    response[3] = 0x83;

    response[6] = 0x00;
    response[7] = 0x00;

    response[8] = 0x00;
    response[9] = 0x00;

    response[10] = 0x00;
    response[11] = 0x00;

    return query_len;
}