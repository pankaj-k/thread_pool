thread_pool
===========

This is an early prototype of a thread pool library I had written using POSIX threads on Linux.
It implements a worker pool model where workers wait for work to appear on the input queue. 
As soon as the work comes the worked crew pick up the job and once done put the result on output queue.
And start waiting for the work again.
The code uses pthread_cond_wait and pthread_mutex extensively to achieve high performance.

The demo test file uses this thread pool library to calculate the Option Call price using the Monte Carlo Method.
