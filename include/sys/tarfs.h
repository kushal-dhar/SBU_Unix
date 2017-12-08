#ifndef _TARFS_H
#define _TARFS_H

#define TARFS_HEADER 512

#define FILETYPE 0
#define DIRECTORY 5

typedef struct tar_file tarfile_t;

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

struct tar_file {
  char name[100];
  char uid[8];
  uint64_t size;
  uint64_t addr;
  uint64_t inode;
  uint64_t p_inode;
  uint64_t type;
};

typedef struct posix_header_ustar tarfs_t;

extern uint64_t is_file_exist(char *filename);
extern int get_parent_inode(char* file);
extern void init_tarfs();
extern int open(char *filename, int permission);
extern int read(int fd, char *buf, int size);
extern void read_dir(int fd);
extern int opendir(char *filename);
extern int  changedir( char* filename);
extern void getcwd();
extern void chdir(char * dir);
#endif
