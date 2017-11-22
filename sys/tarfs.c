#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include <sys/kb.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/utils.h>
#include <sys/string.h>

tarfile_t global_tarfs[100];
int file_count = 0;
char temp_filename[100];

uint64_t is_file_exist(char *filename) {
    tarfs_t *tarfs_start = (tarfs_t *)&_binary_tarfs_start;
    int      offset    = 0;
    int      size      = 0;

    while (strlen(tarfs_start->name) != 0) {
	kprintf("file: %s ",tarfs_start->name);
 	size = atoi(tarfs_start->size);
        size = octtodec(size);
        if (strcmp(tarfs_start->name, filename) == 0) {
	    return offset + TARFS_HEADER;
	}
        else if ( size == 0) {
	    offset += TARFS_HEADER;
        }
	else {
	    if (size % TARFS_HEADER == 0) {
		offset += size + TARFS_HEADER;
	    } 
	    else {
		offset += ((size / TARFS_HEADER) + 1) * TARFS_HEADER + TARFS_HEADER;
	    }
	}
	tarfs_start = (tarfs_t *)(&_binary_tarfs_start + offset);
    }
    return 0;
}


/*
 * Get the parent inode number for the current file
 */
int get_parent_inode(char* file) {
//    char *temp_filename[100] = {'\0'};
    int len  = strlen(file);

    len --;
    strcpy(file, temp_filename);
    while (file[len] != '\\') {
	len --;
	if (len == 0) {
	    return -1;
	}
    }
    temp_filename[++len] = '\0';
    
    len = 0;
    while(len < file_count) {
	if (strcmp(temp_filename, global_tarfs[len].name) == 0) {
	    return len;
	}
	len ++;
    }
    return -1;
}


/*
 * Iterate through tarfs file and maintain own file directory
 */
void init_tarfs() {
    int      index      = 0;
    int      offset     = 0;
    int      size       = 0;
    int      int_val    = 0;
    tarfs_t *tarfs_iter = (tarfs_t *)&_binary_tarfs_start;

    while (strlen(tarfs_iter->name) != 0) {
	size = atoi(tarfs_iter->size);
	size = octtodec(size);

	strcpy(tarfs_iter->name, global_tarfs[index].name);
//	global_tarfs[index].mode = tarfs_iter->mode;
  	global_tarfs[index].size = size;
        int_val = atoi(tarfs_iter->typeflag);
	
//	if (int_val == DIRECTORY) {
	    int_val = get_parent_inode(global_tarfs[index].name);
	    if (int_val == -1) {
		global_tarfs[index].p_inode = 0;
	    }
	    else {
		global_tarfs[index].p_inode = int_val;
	    }
//	}
	global_tarfs[index].type = int_val;
   	global_tarfs[index].addr = (uint64_t)&_binary_tarfs_start + offset + TARFS_HEADER;
	global_tarfs[index].inode = ++file_count;
	
        if ( size == 0) {
            offset += TARFS_HEADER;
        }
        else {
            if (size % TARFS_HEADER == 0) {
                offset += size + TARFS_HEADER;
            }
            else {
                offset += ((size / TARFS_HEADER) + 1) * TARFS_HEADER + TARFS_HEADER;
            }
        }
  	index ++;
        tarfs_iter = (tarfs_t *)(&_binary_tarfs_start + offset);
    }
    return;
}

/*
 * Stub to open file contents
 */
int open(char *filename, int permission) {
    int     iterator    = 0;

    while (iterator < file_count) {
	if (strcmp(filename, global_tarfs[iterator].name) == 0) {
	    return global_tarfs[iterator].inode;
	}
	iterator ++;
    }
    return -1;
}

/*
 * Stub to read file contents
 */
int read(int fd, char *buf, int size) {
    int      length      = 0;
    int      iterator    = 0;
    char    *content;
    
    while (iterator < file_count) {
	if (global_tarfs[iterator].inode == fd) {
	    fd = iterator;
	    break;
	}
	iterator++;
    }
    length = global_tarfs[fd].size;
    content = (char *)global_tarfs[fd].addr;

    length = (length > size)?size:length;
    memcpy((void *)content, (void *)buf, length);

    return length;
} 

/*
 * Stub to close file object. Not performing any task here as only 1 process
 * can access the file in cooperative scheduling
 */ 
int close(int fd) {
    return 1;
}

/*
 * Stub to open directory
 */
int opendir(char *filename) {
    int     iterator    = 0;

    while (iterator < file_count) {
        if ((global_tarfs[iterator].type == DIRECTORY) && (strcmp(filename, global_tarfs[iterator].name) == 0)) {
            return global_tarfs[iterator].inode;
        }
        iterator ++;
    }
    return -1;

}

/*
 * Stub to read file contents
 */
int read_dir(int fd, char *buf, int size) {
    int      length      = 0;
    int      iterator    = 0;
    char    *content;

    while (iterator < file_count) {
        if (global_tarfs[iterator].inode == fd) {
            fd = iterator;
            break;
        }
        iterator++;
    }
    length = global_tarfs[fd].size;
    content = (char *)global_tarfs[fd].addr;

    length = (length > size)?size:length;
    memcpy((void *)content, (void *)buf, length);

    return length;
}

int close_dir(int fd) {
    return 1;
}

