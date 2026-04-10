//
// Created by thehall16 on 4/5/26.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "forwarder.h"

#define UPSTREAM_DNS "8.8.8.8"
#define DNS_PORT 53

int forward_dns_query(unsigned char *query, int query_len, unsigned char *response, int response_size) {
    int n;

    struct sockaddr_in upstream_addr;
    struct timeval timeout;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Forwarder socket creation failed\n");
        return -1;
    }

    memset(&upstream_addr, 0, sizeof(upstream_addr));
    upstream_addr.sin_family = AF_INET;
    upstream_addr.sin_port = htons(DNS_PORT);

    if (inet_pton(AF_INET, UPSTREAM_DNS, &upstream_addr.sin_addr) <= 0) {
        perror("Invalid upstream DNS address\n");
        close(sockfd);
        return -1;
    }

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Setsockopt failed\n");
        close(sockfd);
        return -1;
    }

    n = sendto(sockfd, query, query_len, 0, (struct sockaddr *)&upstream_addr, sizeof(upstream_addr));

    if (n < 0) {
        perror("Forwarder sendto failed\n");
        close(sockfd);
        return -1;
    }

    n = recvfrom(sockfd, response, response_size, 0, NULL, NULL);
    if (n < 0) {
        perror("Forwarder recvfrom failed\n");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return n;
}
