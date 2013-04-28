#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "ThreadObject.h"
#include<vector>

using namespace std;


class thread_pool
{
private:
	vector<ThreadObject*> worker;
	int numOfThreads;	//Initial number of threads which have to created.
public:
	thread_pool(int x, FPTR f);
	~thread_pool();
	int startPool();
	int shutDownPool();
};

int queue(void *arg);
void* result();
#endif
