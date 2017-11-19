#ifndef _FILE_H
#define _FILE_H

typedef struct file_struct file_t;

struct file_struct{
    char     name[100];
    int      inode;
    int      access;
    int      size;
    char     is_open;
    uint64_t address;
};

#endif
