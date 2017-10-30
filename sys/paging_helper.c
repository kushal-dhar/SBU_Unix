#include <sys/defs.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <string.h>

int get_index(uint64_t address, int offset) {
    address = (address >> offset) & 0x1FF;
    return address;
}
