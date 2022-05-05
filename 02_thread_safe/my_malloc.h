#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // brk, sbrk
#include <pthread.h> // pthread_mutex_lock, pthread_mutex_unlock


#define HEADER_SIZE sizeof(block)

typedef struct block_t {
  size_t size;
  struct block_t * prev;
  struct block_t * next;
} block;

void * bf_malloc(size_t size);
void bf_free(void * ptr);

// Sub functions
block * extendBlock(size_t size);
block * splitAndupdateBlock(block * cur, size_t size);
block * mergeBlock(block * former, block * latter);
void addBlock(block * cur);
void removeBlock(block * cur);
void sub_free(void * ptr);
block * bf_find(size_t size);

//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version 
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);


#endif


