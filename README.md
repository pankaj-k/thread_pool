thread_pool
===========

This is a Thread Pool Library I created on Linux using POSIX threads.
To keep it simple, It creates a user configured number of threads at the start of the library.
The threads wait on the inout queue for the data to appear.
The data when it comes is consumed and the output is put on the output queue.
The threads use pthread_cond_wait and pthread_mutex facilities rather extensively.
