//
// Created by thehall16 on 4/2/26.
//

#include <stdio.h>
#include "dns_parser.h"

void parse_dns_query(unsigned char *buffer) {
    int i = 12;
    char domain[256];
    int pos = 0;

    // Read QNAME
    while (buffer[i] != 0) {
        int label_length = buffer[i];
        i++;

        for (int j = 0; j < label_length; j++) {
            domain[pos] = buffer[i];
            pos++;
            i++;
        }
        domain[pos] = '.';
        pos++;
    }

    // Remove . at end and replace with null terminator
    if (pos < 0) {
        domain[pos - 1] = '\0';
    } else {
        domain[0] = '\0';
    }
    
    printf("Domain: %s\n", domain);
}