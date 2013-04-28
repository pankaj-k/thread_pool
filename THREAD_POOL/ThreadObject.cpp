#include "ThreadObject.h"

#include<iostream>
#include<cerrno>	//For ETIMEDOUT variable
using namespace std;

extern pthread_mutex_t lock_1;		//
extern pthread_cond_t added;		//
extern pthread_mutex_t work_lock;	// Defined 
extern pthread_mutex_t result_lock;	// in
extern pthread_cond_t work_added;	// thread_pool.cpp
extern pthread_cond_t result_added;	// all
extern std::deque<void*> work_queue;	// these	
extern std::deque<void*> result_queue;	// variables
extern std::deque<pthread_t> toBeJoined;//


int ThreadObject::Activate()
{
	int res;
	res=pthread_create(&threadID, NULL, thread_func,static_cast<void*>(this));
	if(res==EAGAIN)
	{	
		string error = "Unable to create threads . System out of resources. Try to lower the number of threads to be created.\n";
		throw runtime_error(error);
		//cout<<"\nWorker Number "<<threadID<<" to failed start. System Out of Resources.\n";
		//return 1;
	}
	isHealthy=1;
	//cout<<"\nThread "<<threadID<<" created.\n";
	threadID_stream<<threadID; 	//Saving a stream type of threadID for logging.
	return 0;
}	


void* ThreadObject::thread_func(void* arg)
{
        ThreadObject *ptr=static_cast<ThreadObject*>(arg);
        ptr->waitForWork();
        return NULL;
}

void ThreadObject::waitForWork()
{
	void* tmp;	//To collect the work.
	while(1)
	{
		//Get work and if not availiable Wait for work
		pthread_mutex_lock(&work_lock);		//Lock work queue.
		while(work_queue.size()==0)
		{
			pthread_cond_wait(&work_added, &work_lock);
	
		}
		tmp=work_queue.front();
		work_queue.pop_front();
		pthread_mutex_unlock(&work_lock);
		if(tmp==NULL)
		{
			//Poison Pill. Shutdown in progress. This thread has to go back to its maker.
			pthread_mutex_lock(&lock_1);
			toBeJoined.push_back(threadID);
			pthread_cond_signal(&added);	//Signal to shutDown function.
			pthread_mutex_unlock(&lock_1);
			pthread_exit(NULL);
		}
       		assignment=tmp;
		tmp=(*fptr)(assignment);   //WORKING. Collect the result in tmp.
		pthread_mutex_lock(&result_lock);       //Adding the result to the result queue	
		result_queue.push_back(tmp);
		pthread_cond_signal(&result_added);     //Need to signal if something was added to result queue.
		pthread_mutex_unlock(&result_lock);
	}
}

pthread_t ThreadObject::getThreadId()const
{	
	return threadID;
}
