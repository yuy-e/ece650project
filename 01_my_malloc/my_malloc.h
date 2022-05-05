#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // brk, sbrk


#define HEADER_SIZE sizeof(block)

typedef struct block_t {
  size_t size;
  struct block_t * prev;
  struct block_t * next;
} block;

// First Fit
void * ff_malloc(size_t size);
void ff_free(void * ptr);

// Best Fit
void * bf_malloc(size_t size);
void bf_free(void * ptr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();

// Sub functions
block * extendBlock(size_t size);
block * splitAndupdateBlock(block * cur, size_t size);
block * mergeBlock(block * former, block * latter);
void addBlock(block * cur);
void removeBlock(block * cur);
void sub_free(void * ptr);
block * bf_find(size_t size);

#endif


