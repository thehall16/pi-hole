//
// Created by thehall16 on 4/2/26.
//
#include <stdio.h>
#include "server.h"
#include "blocklist.h"

int main(void) {
    if (load_blocklist("../config/blocklist.txt") != 0) {
        return 1;
    }

    start_dns_server();
    free_blocklist();
    return 0;
}