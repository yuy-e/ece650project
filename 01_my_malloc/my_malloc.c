#include "my_malloc.h"

// start of Heap
static block * head = NULL;
// track of entire heap memory
unsigned long data_segment = 0;
// track of free memory
unsigned long free_space_segment = 0;

// Print free list
void printlist() {
  printf("start\n");
  block * tmp = head;
  if (tmp) {
    printf("%ld\n", tmp->size);
    while (tmp->next) {
      printf("%ld\n", tmp->next->size);
      tmp = tmp->next;
    }

    printf("反向\n");
    while (tmp->prev) {
      printf("%ld\n", tmp->prev->size);
      tmp = tmp->prev;
    }
  }
}

/* Extend heap. */
block * extendBlock(size_t size) {
  block * newBlock = (block *)sbrk(size + HEADER_SIZE);
  if (newBlock == (void *)-1) {
    return NULL;
  }
  newBlock->size = size;
  newBlock->prev = NULL;
  newBlock->next = NULL;

  // Update Heap Memory
  data_segment = data_segment + size + HEADER_SIZE;

  return newBlock;
}

/* Split Block that has extra free space,remove and add extra to Free List. */
block * splitAndupdateBlock(block * cur, size_t size) {
  block * newBlock = (block *)((void *)cur + HEADER_SIZE + size);
  // removeBlock(cur);
  if (cur->prev) {
    cur->prev->next = newBlock;
  }
  else {
    head = newBlock;
  }
  newBlock->prev = cur->prev;
  if (cur->next) {
    cur->next->prev = newBlock;
  }
  newBlock->next = cur->next;
  newBlock->size = cur->size - size - HEADER_SIZE;
  cur->size = size;
  cur->prev = NULL;
  cur->next = NULL;
  // Update Free Space
  free_space_segment = free_space_segment - size - HEADER_SIZE;

  return newBlock;
}

/* Merge two free adjacent blocks */
block * mergeBlock(block * former, block * latter) {
  if (!former) {
    return latter;
  }
  if (!latter) {
    return former;
  }
  unsigned long former_end = (unsigned long)former + former->size + HEADER_SIZE;
  unsigned long latter_start = (unsigned long)latter;
  if (former_end == latter_start) {
    former->next = latter->next;
    if (latter->next) {
      latter->next->prev = former;
    }
    latter->prev = NULL;
    latter->next = NULL;
    former->size = former->size + latter->size + HEADER_SIZE;
    return former;
  }
  return latter;
}

/* Find proper position to add Free Block. */
void addBlock(block * cur) {
  if (!head) {
    head = cur;
    cur->prev = NULL;
    cur->next = NULL;
  }
  else if ((unsigned long)head > (unsigned long)cur) {
    head->prev = cur;
    cur->next = head;
    cur->prev = NULL;
    head = cur;
  }
  else {
    // Search for proper position to insert block
    block * search_pointer = head;
    block * search_pointer_prev = NULL;
    while (search_pointer != NULL &&
           ((unsigned long)search_pointer < (unsigned long)cur)) {
      search_pointer_prev = search_pointer;
      search_pointer = search_pointer->next;
    }
    cur->next = search_pointer;
    cur->prev = search_pointer_prev;
    search_pointer_prev->next = cur;
    if (search_pointer) {
      search_pointer->prev = cur;
    }
  }
  // Update Free Space
  free_space_segment = free_space_segment + cur->size + HEADER_SIZE;
  block * first_two = mergeBlock(cur->prev, cur);
  block * last_two =mergeBlock(first_two, cur);
}

/* Remove Free Block from Free List. */
void removeBlock(block * cur) {
  if (cur->prev) {
    cur->prev->next = cur->next;
    if (cur->next) {
      cur->next->prev = cur->prev;
    }
  }
  else if (cur->next) {
    head = cur->next;
    head->prev = NULL;
  }
  else {
    head = NULL;
  }
  cur->prev = NULL;
  cur->next = NULL;

  // Update Free Space
  free_space_segment = free_space_segment - cur->size - HEADER_SIZE;
}

/* FIRST FIT MALLOC */
void * ff_malloc(size_t size) {
  //printf("Malloc: %zu\n", size);
  block * cur = head;
  while (cur != NULL) {
    if (cur->size >= size) {
      if (cur->size > size + HEADER_SIZE) {
        splitAndupdateBlock(cur, size);
      }
      else {
        removeBlock(cur);
      }
      void * data_cur = (void *)cur + HEADER_SIZE;
      // printlist()
      return data_cur;
    }
    cur = cur->next;
  }
  // Extend Memory
  // printlist()
  block * temp = extendBlock(size);
  void * data_cur = (void *)((unsigned long)temp + HEADER_SIZE);
  return data_cur;
}

/* Free */
void sub_free(void * ptr) {
  block * free_ptr = (block *)(ptr - HEADER_SIZE);
  addBlock(free_ptr);
  // printlist()
}

/* FIRST FIT Free */
void ff_free(void * ptr) {
  sub_free(ptr);
}

/* Size of (actual usable free space + space occupied by metadata) of free blocks */
unsigned long get_data_segment_size() {
  return data_segment;
}

/* Entire heap memory(that includes memory used to save metadata) */
unsigned long get_data_segment_free_space_size() {
  return free_space_segment;
}

/* Find blocks that satify requirements according best fit principle. */
block * bf_find(size_t size) {
  block * cur = head;
  size_t extraSpace = INT_MAX;
  block * ans = NULL;

  while (cur) {
    if (cur->size > size + HEADER_SIZE) {
      if ((cur->size - size - HEADER_SIZE) < extraSpace) {
        ans = cur;
        extraSpace = cur->size - size - HEADER_SIZE;
      }
      cur = cur->next;
    }
    else if(cur->size >= size){
      ans = cur;
      return ans;
    }
    else {
      cur = cur->next;
    }
  }
  return ans;
}

/* BEST FIT MALLOC */
void * bf_malloc(size_t size) {
  block * ans = bf_find(size);
  if (ans) {
    //printf("Find Reused Block\n");
    if (ans->size > size + HEADER_SIZE) {
      splitAndupdateBlock(ans, size);
    }
    else {
      removeBlock(ans);
    }
    void * data_ans = (void *)ans + HEADER_SIZE;
    // printlist()
    return data_ans;
  }
    //printf("Extend Memory\n");
    // Extend Memory
    block * temp = extendBlock(size);
    void * data_cur = (void *)((unsigned long)temp + HEADER_SIZE);
    // printlist()
    return data_cur;
}

/* BEST FIT FREE */
void bf_free(void * ptr) {
  sub_free(ptr);
}