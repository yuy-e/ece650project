#include "my_malloc.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nolock = PTHREAD_MUTEX_INITIALIZER;

// start of Heap
static block * head = NULL;
__thread block * global_head = NULL; // TLS

/* Extend heap. */
block * extendBlock(size_t size) {
  pthread_mutex_lock(&nolock);
  block * newBlock = (block *)sbrk(size + HEADER_SIZE);
  pthread_mutex_unlock(&nolock);
  if (newBlock == (void *)-1) {
    //pthread_mutex_unlock(&lock);
    return NULL;
  }
  newBlock->size = size;
  newBlock->prev = NULL;
  newBlock->next = NULL;

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

  return newBlock;
}

/* Split Block that has extra free space,remove and add extra to Free List. */
block * splitAndupdateBlock_tls(block * cur, size_t size) {
  block * newBlock = (block *)((void *)cur + HEADER_SIZE + size);
  // removeBlock(cur);
  if (cur->prev) {
    cur->prev->next = newBlock;
  }
  else {
    global_head = newBlock;
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

  block * first_two = mergeBlock(cur->prev, cur);
  block * last_two = mergeBlock(first_two, cur);
}

/* Find proper position to add Free Block. */
void addBlock_tls(block * cur) {
  if (!global_head) {
    global_head = cur;
    cur->prev = NULL;
    cur->next = NULL;
  }
  else if ((unsigned long)global_head > (unsigned long)cur) {
    global_head->prev = cur;
    cur->next = global_head;
    cur->prev = NULL;
    global_head = cur;
  }
  else {
    // Search for proper position to insert block
    block * search_pointer = global_head;
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

  block * first_two = mergeBlock(cur->prev, cur);
  block * last_two = mergeBlock(first_two, cur);
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
}

/* Remove Free Block from Free List. */
void removeBlock_tls(block * cur) {
  if (cur->prev) {
    cur->prev->next = cur->next;
    if (cur->next) {
      cur->next->prev = cur->prev;
    }
  }
  else if (cur->next) {
    global_head = cur->next;
    global_head->prev = NULL;
  }
  else {
    global_head = NULL;
  }
  cur->prev = NULL;
  cur->next = NULL;
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
    else if (cur->size >= size) {
      ans = cur;
      return ans;
    }
    else {
      cur = cur->next;
    }
  }
  return ans;
}

/* Find blocks that satify requirements according best fit principle. */
block * bf_find_tls(size_t size) {
  block * cur = global_head;
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
    else if (cur->size >= size) {
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
  block * free_ptr = (block *)(ptr - HEADER_SIZE);
  addBlock(free_ptr);
}

//Thread Safe malloc/free: locking version
void * ts_malloc_lock(size_t size) {
  // Acquire the lock
  pthread_mutex_lock(&lock);
  block * ans = bf_find(size);
  if (ans) {
    if (ans->size > size + HEADER_SIZE) {
      splitAndupdateBlock(ans, size);
    }
    else {
      removeBlock(ans);
    }
    void * data_ans = (void *)ans + HEADER_SIZE;
    // Release the lock
    pthread_mutex_unlock(&lock);
    return data_ans;
  }
  else {
    block * temp = extendBlock(size);
    if (temp == NULL) {
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    void * data_cur = (void *)((unsigned long)temp + HEADER_SIZE);
    // Release the lock
    pthread_mutex_unlock(&lock);
    return data_cur;
  }
}

void ts_free_lock(void * ptr) {
  // Acquire the lock
  pthread_mutex_lock(&lock);
  block * free_ptr = (block *)(ptr - HEADER_SIZE);
  addBlock(free_ptr);
  // Release the lock
  pthread_mutex_unlock(&lock);
}

//Thread Safe malloc/free: non-locking version
void * ts_malloc_nolock(size_t size) {
  block * ans = bf_find_tls(size);
  if (ans) {
    if (ans->size > size + HEADER_SIZE) {
      splitAndupdateBlock_tls(ans, size);
    }
    else {
      removeBlock_tls(ans);
    }
    void * data_ans = (void *)ans + HEADER_SIZE;
    return data_ans;
  }
  else {
    block * temp = extendBlock(size);
    if (temp == NULL) {
      return NULL;
    }
    void * data_cur = (void *)((unsigned long)temp + HEADER_SIZE);
    return data_cur;
  }
}

void ts_free_nolock(void * ptr) {
  block * free_ptr = (block *)(ptr - HEADER_SIZE);
  addBlock_tls(free_ptr);
}