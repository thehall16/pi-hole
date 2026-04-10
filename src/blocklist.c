//
// Created by thehall16 on 4/2/26.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blocklist.h"

#define MAX_DOMAINS 1000
#define MAX_DOMAIN_LENGTH 256

static char *blocked_domains[MAX_DOMAINS];
static int blocked_count = 0;

static void trim_newline(char *str) {
    int len = strlen(str);

    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}
static int domain_matches(const char *domain, const char *blocked) {
    int domain_len = strlen(domain);
    int blocked_len = strlen(blocked);

    // If domain is exact match
    if (strcmp(domain, blocked) == 0) {
        return 1;
    }

    // Ensure blocked entry is not longer than query domain
    if (blocked_len >= domain_len) {
        return 0;
    }

    // Check if end of queried domain matched blocklist entry
    if (strcmp(domain + (domain_len - blocked_len), blocked) != 0) {
        return 0;
    }

    // Ensure there is a real subdomain boundary before the suffix
    if (domain[domain_len - blocked_len - 1] == '.') {
        return 1;
    }

    return 0;
}
int load_blocklist(const char *filename) {
    FILE *file;
    char line[MAX_DOMAIN_LENGTH];

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening blocklist\n");
        return -1;
    }

    blocked_count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        trim_newline(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        if (blocked_count >= MAX_DOMAINS) {
            printf("Blocklist limit reach\n");
            break;
        }

        blocked_domains[blocked_count] = malloc(strlen(line) + 1);
        if (blocked_domains[blocked_count] == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            return -1;
        }

        strcpy(blocked_domains[blocked_count], line);
        blocked_count++;
    }

    fclose(file);

    printf("Loaded %d blocked domains\n", blocked_count);
    return 0;
}

int is_blocked(const char *domain) {
    for (int i = 0; i < blocked_count; i++) {
        if (domain_matches(domain, blocked_domains[i])) {
            return 1;
        }
    }

    return 0;
}

void free_blocklist(void) {
    for (int i = 0; i < blocked_count; i++) {
        free(blocked_domains[i]);
        blocked_domains[i] = NULL;
    }

    blocked_count = 0;
}