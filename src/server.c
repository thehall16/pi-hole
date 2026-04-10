//
// Created by thehall16 on 4/2/26.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "dns_parser.h"
#include "blocklist.h"
#include "server.h"
#include "dns_response.h"
#include "forwarder.h"

#define PORT 8053
#define BUFFER_SIZE 512

#define BLOCK_MODE BLOCK_NULL_IP // <--- Change this to BLOCK_NULL_IP or BLOCK_NXDOMAIN

void start_dns_server(void) {
    int response_len;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    unsigned char buffer[BUFFER_SIZE];
    unsigned char response[BUFFER_SIZE];
    char domain[256];

    // Create UDP Socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created\n");

    // Set up server addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to port 53
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed\n");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening on port %d...\n", PORT);

    // Main loop
    while (1) {
        const int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (n < 0) {
            perror("recvfrom failed\n");
            continue;
        }

        // Clear domain and response buffers before parsing
        memset(domain, 0, sizeof(domain));
        memset(response, 0, sizeof(response));

        // Parse domain from query
        parse_dns_query(buffer, domain);



        printf("\nReceived DNS query (%d bytes)\n", n);
        printf("From IP: %s\n", inet_ntoa(client_addr.sin_addr));
        printf("Domain: %s\n", domain);

        if (is_blocked(domain)) {
            printf("Status: BLOCKED\n");

            if (BLOCK_MODE == BLOCK_NXDOMAIN) {
                response_len = build_nxdomain_response(buffer, response, n);
                printf("Blocking mode: NXDOMAIN\n");
            } else {
                response_len = build_null_ip_response(buffer, response, n);
                printf("Blocking mode: NULLIP (0.0.0.0)\n");
            }

            if (sendto(sockfd, response, response_len, 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
                perror("sendto failed\n");
            } else {
                printf("Sent blocked response\n");
            }
        } else {
            printf("Status: ALLOWED\n");

            response_len = forward_dns_query(buffer, n, response, BUFFER_SIZE);

            if (response_len < 0) {
                perror("Failed to get upstream response\n");
                continue;
            }

            if (sendto(sockfd, response, response_len, 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
                perror("Sendto failed (server)\n");
            } else {
                printf("Forwarded upstream response\n");
            }
        }
    }
    close(sockfd);

}