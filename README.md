# ece650project
Assignments and Projects of Duke Course ECE650

Project #1: Implemented my version of several memory allocation functions: malloc(), free() based on two different memory allocation policies: First Fit and Best Fit, and analyzed their performance in terms of run-time of a program and fragmentation. 

Project #2: Implemented two different thread-safe versions of malloc() and free() functions and tested in a UNIX-based environent(Ubuntu 18.04). 
In Version1, I used lock-based synchronization to prevent race conditions based on pthread library and needed synchronization primitives. 
In Version2, I acquire a lock before and release the lock after calling sbrk immediately since the the sbrk function is not thread-safe.

Project #3: Developed a pair of game that interact to model a hot-potato game using TCP sockets. The ringmaster creates a "potato" object and send it to randomly selected player, the player who receive the "potato" will decrement the number of hops of the "potato" and transfer it to a randomly selected neighbor player until the number of hops reaches zero. Then the player would send the "potato" back to the ringmaster and also print out the trace of the "potato".

Project #4: Database Programming
Built a PostgreSQL database and implements create, insert, query functions through two different ways: SQL sentencess 
