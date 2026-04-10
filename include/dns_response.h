//
// Created by thehall16 on 4/5/26.
//

#ifndef DNS_RESPONSE_H
#define DNS_RESPONSE_H
    typedef enum {
        BLOCK_NXDOMAIN,
        BLOCK_NULL_IP
    } block_mode_t;
    int build_nxdomain_response(unsigned char *query, unsigned char *response, int query_len);
    int build_null_ip_response(unsigned char *query, unsigned char *response, int query_len);
#endif //DNS_RESPONSE_H
