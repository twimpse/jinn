// data_storage.h
#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <time.h>
#include <unistd.h>
#include "user_db.h"

// Permission bits
#define PERM_READ   0x01
#define PERM_WRITE  0x02
#define PERM_EXEC   0x04


// Access control struct
struct access_rights {
    int min_user_level;
    int owner_group;
    unsigned int read_mask;
    unsigned int write_mask;
    unsigned int exec_mask;
};

// File entry struct
struct file_entry {
    char *filename;
    void *data;
    size_t size;
    time_t created;
    time_t last_access;
    unsigned int times_accessed;
    int used;
    struct access_rights access;
    uid_t owner_id;
};

// Function prototypes
int file_can_add(size_t file_size);
void file_storage_info(int *file_count, size_t *total_bytes);

int file_add(const char *filename, const void *data, size_t size, 
             int min_level, int group, unsigned int read_mask, 
             unsigned int write_mask, unsigned int exec_mask, uid_t owner);

struct file_entry *file_read(const char *filename, user_t *user);
int file_delete(const char *filename, user_t *user);
int file_set_permissions(const char *filename, user_t *user, 
                         int min_level, int group, 
                         unsigned int read_mask, 
                         unsigned int write_mask, 
                         unsigned int exec_mask);

char **file_list(void);
void file_list_free(char **list);
char **file_search(const char *pattern);
int can_access(struct file_entry *f, user_t *u, int op);

#endif
