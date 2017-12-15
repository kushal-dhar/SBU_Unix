#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libc.h>


void* malloc(int size){
  uint64_t  ch;
  ch = syscall_2(9,(uint64_t) size);
  return (void *)ch;
}
