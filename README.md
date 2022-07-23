# ece650project
Assignments and Projects of Duke Course ECE650

**Project #1:** Implemented my version of several memory allocation functions: malloc(), free() based on two different memory allocation policies: First Fit and Best Fit, and analyzed their performance in terms of the run-time of a program and fragmentation.


**Project #2:** Implemented two different thread-safe versions of malloc() and free() functions and tested them in a UNIX-based environment(Ubuntu 18.04). In Version1, I used lock-based synchronization to prevent race conditions based on the pthread library and needed synchronization primitives. In Version2, I acquire a lock before and release the lock after calling sbrk immediately since the sbrk function is not thread-safe.


**Project #3:** Developed a game that interacts to model a hot-potato game using TCP sockets. The ringmaster creates a "potato" object and sends it to a randomly selected player, the player who receives the "potato" will decrement the number of hops of the "potato" and transfer it to a randomly selected neighbor player until the number of hops reaches zero. Then the player would send the "potato" back to the ringmaster and also print out the trace of the "potato".


**Project #4:** Database Programming builts a PostgreSQL database and implements create, insert, and query functions in two different ways: SQL sentences and ORM(Django)

**Project #5:** Attack operating system using Linux Rootkit, including copying files, hiding files and directories, loading and unloading sneaky modules, and hiding sneaky modules and processes.  
