
pthread_mutex_t global_malloc_lock;

void * ts_malloc_lock(size_t size) {
  // Acquire the lock
  pthread_mutex_lock(&global_malloc_lock);
  block * ans = bf_find(size);
  if (ans) {
    if (ans->size > size + HEADER_SIZE) {
      splitAndupdateBlock(ans, size);
    }
    else {
      removeBlock(ans);
    }
    void * data_ans = (void *)ans + HEADER_SIZE;
    pthread_mutex_unlock(&global_malloc_lock);
    return data_ans;
  }
  else {
    block * temp = extendBlock(size);
    void * data_cur = (void *)((unsigned long)temp + HEADER_SIZE);
    pthread_mutex_unlock(&global_malloc_lock);
    return data_cur;
  }
}
