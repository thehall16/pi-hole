//
// Created by thehall16 on 4/2/26.
//

#ifndef BLOCKLIST_H
#define BLOCKLIST_H
    int load_blocklist(const char *filename);
    int is_blocked(const char *domain);
    void free_blocklist(void);
#endif //BLOCKLIST_H
