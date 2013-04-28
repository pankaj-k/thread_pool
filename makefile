CPP	= g++
CFLAGS 	= -g

all:	testBinary

testBinary:	ThreadPool
	g++ -o testBinary Test.cpp  -L./THREAD_POOL -lThreadPool -lpthread -lrt

clean:
	cd THREAD_POOL;rm -rf *.a
	cd THREAD_POOL;rm -rf *.o

ThreadPool: ThreadObject.o thread_pool.o 
	cd THREAD_POOL;ar crv libThreadPool.a ThreadObject.o thread_pool.o 
	cd THREAD_POOL;ranlib libThreadPool.a

ThreadObject.o:
	cd THREAD_POOL;$(CPP) $(CFLAGS) -c ThreadObject.cpp

thread_pool.o:
	cd THREAD_POOL;$(CPP) $(CFLAGS) -c thread_pool.cpp

 
