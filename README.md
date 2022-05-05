# ece650project
Assignments and Projects of Duke Course ECE650

Project #1: Implemented my version of several memory allocation functions: malloc(), free() based on two different memory allocation policies: First Fit and Best Fit, and analyzed their performance in terms of run-time of a program and fragmentation. 

Project #2: Implemented two different thread-safe versions of malloc() and free() functions and tested in a UNIX-based environent(Ubuntu 18.04). 
In Version1, I used lock-based synchronization to prevent race conditions based on pthread library and needed synchronization primitives. 
In Version2, I acquire a lock before and release the lock after calling sbrk immediately since the the sbrk function is not thread-safe.

