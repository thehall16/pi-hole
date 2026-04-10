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

int build_null_ip_response(unsigned char *query, unsigned char *response, int query_len) {
    memcpy(response, query, query_len);

    // Find end of QNAME
    int i = 12;
    while (response[i] != 0) {
        i += response[i] + 1;
    }

    // Skip null byte at end of QNAME
    i++;

    // Skip QTYPE (2 bytes) and QCLASS (2 bytes)
    i += 4;

    int answer_start = i;

    // Standard successful response
    // QR = 1, RD = 1, RA = 1, RCODE = 0
    response[2] = 0x81;
    response[3] = 0x80;

    // QDCOUNT stays the same
    // ANCOUNT = 1
    response[6] = 0x00;
    response[7] = 0x01;

    // NSCOUNT = 0
    response[8] = 0x00;
    response[9] = 0x00;

    // ARCOUNT = 0
    response[10] = 0x00;
    response[11] = 0x00;

    // Answer section
    // NAME = pointer to original QNAME at offset 12
    response[answer_start++] = 0xC0;
    response[answer_start++] = 0x0C;

    // TYPE = A
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x01;

    // CLASS = IN
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x01;

    // TTL = 60 seconds
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x3C;

    // RDLENGTH = 4 bytes
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x04;

    // RDATA = 0.0.0.0
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x00;
    response[answer_start++] = 0x00;

    const int response_len = answer_start;
    return response_len;
}