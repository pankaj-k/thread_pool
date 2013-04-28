#include "thread_pool.h"
#include<iostream>

std::deque<pthread_t> toBeJoined; //Single copy for all ThreadObject's ThreadID's.
std::deque<void*> work_queue;
std::deque<void*> result_queue;

pthread_cond_t added;
pthread_cond_t work_added;
pthread_cond_t result_added;

pthread_mutex_t lock_1;
pthread_mutex_t work_lock;
pthread_mutex_t result_lock;

bool isShutDown=false;

thread_pool::thread_pool(int x, FPTR f):numOfThreads(x)
{
	pthread_cond_init(&added, NULL);
	pthread_cond_init(&work_added,NULL);
	pthread_cond_init(&result_added,NULL);

	pthread_mutex_init(&lock_1,NULL);
	pthread_mutex_init(&work_lock,NULL);
	pthread_mutex_init(&result_lock,NULL);
	
	for(int i=0; i<numOfThreads; ++i)
	{
		worker.push_back(new ThreadObject(f));
	}
}

thread_pool::~thread_pool()
{
	for( vector<ThreadObject*>::iterator iter=worker.begin(); iter!=worker.end(); ++iter)
	{
		delete (*iter);
	}
	worker.clear();
	pthread_cond_destroy(&added);
	pthread_cond_destroy(&work_added);
	pthread_cond_destroy(&result_added);

	pthread_mutex_destroy(&lock_1);
	pthread_mutex_destroy(&work_lock);
	pthread_mutex_destroy(&result_lock);
}

int thread_pool::startPool()
{
	try
	{
	for( vector<ThreadObject*>::iterator iter=worker.begin(); iter!=worker.end(); ++iter)
	{
		(*iter)->Activate();
	}
	}catch(const runtime_error& err)
		{
			cout<<err.what();
			shutDownPool();
			string error="\nThread pool library reports an error. Shutting down now !!\n";
			throw runtime_error(error);
		}			
	return 0;
}


int queue(void *arg)
{
	pthread_mutex_lock(&work_lock);
	if(isShutDown)
	{
		pthread_mutex_unlock(&work_lock);
		return -1;
	}
	work_queue.push_back(arg);
	pthread_cond_signal(&work_added);	//Signal to waiting threads that work has been added.
	pthread_mutex_unlock(&work_lock);
}

void* result()
{
	void *tmp;
	pthread_mutex_lock(&result_lock);
	while(result_queue.size() == 0)
	{
		pthread_cond_wait(&result_added,&result_lock);
	}
	tmp=result_queue.front();
	result_queue.pop_front();
	pthread_mutex_unlock(&result_lock);
	return tmp;
}

int thread_pool::shutDownPool()
{
	int healthyCount=0;
	pthread_t tmp;
        for( vector<ThreadObject*>::iterator iter=worker.begin(); iter!=worker.end(); ++iter)
        {	
		if( (*iter)->getHealth() == 1)	//Find how many threads are actually running.Sometimes thread creation fails.
		{
			++healthyCount;		
		}
        }

	pthread_mutex_lock(&work_lock);
	isShutDown=true;	//Now the thread pool library will not take user input anymore.
	for(int i=0; i<healthyCount; ++i)
	{	
		work_queue.push_back(NULL);	//Poison for Blind Rats.			
		pthread_cond_signal(&work_added);	//Let Rats know that more food has come.
	}
	pthread_mutex_unlock(&work_lock);
	
	
	for(int i=0; i<healthyCount; ++i) //We loop as many times as there are actually threads to make sure we joined all of them
	{
		pthread_mutex_lock(&lock_1);	//Lock before you check the size of to be joined Q of threadID's
		//Is it empty now?
		while(toBeJoined.size()==0)					//If yes
			pthread_cond_wait(&added, &lock_1);	//Sleep till someone adds something to the Q
		tmp=toBeJoined.front();		//Someone added something. Fetch it.
		toBeJoined.pop_front();		//Remove it from the Q
		pthread_mutex_unlock(&lock_1);			//Unlock the Q so that others can add more to be joined ID's.
		std::ostringstream tmp_str;
		tmp_str<<tmp;//fresh stream each time. Streams can have lots of internal state. Resetting all of that takes at least as much code as the stream constructor
		//std::cout<<"\nJoined "<<tmp<<" thread.\n";
		pthread_join(tmp,NULL);				//Meanwhile join the threadID.
	}
	//Need to push a NULL to the result_queue. The client code reading data from the  "void* result()" function will have to 
	//check for NULL. If NULL is there it means that all threads are now shutdown and no more input. So it should use that to 
	//break out of the loop for reading data from the thread library output buffer. Also the NULL will make the result 
	//function stop waiting on conditional variable.
	pthread_mutex_lock(&result_lock);
	result_queue.push_back(NULL);
	pthread_cond_signal(&result_added);
	pthread_mutex_unlock(&result_lock);
        return 0;
}

