#ifndef THREADOBJECT_H
#define THREADOBJECT_H

extern "C" {
#include<unistd.h>
#include<stdlib.h>
}
#include<cstdio>
#include<deque>
#include<sstream>
#include<stdexcept>

typedef void* (*FPTR)(void*);

extern pthread_cond_t added;	//Want them to be visible in the files which include this header file. Hence declaring.
				//Because in header file only defination is allowed making it extern and putting the defination
extern pthread_mutex_t lock_1;	//in the ThreadObject.cpp

class ThreadObject
{
	private:
		pthread_t threadID;
		std::ostringstream threadID_stream;
		int isHealthy;	//If the user specified number of threads to be created is more than the system limit or available resources then the pthread_create calls fail with status EAGAIN. In that case this particular ThreadObject will be marked as such. This will help us to find how many ThreadObject actually were successful in creating thread and running. Then we will call only that many times the thread_join function. 
		void *exit_status;
		FPTR fptr;
		void* assignment;	
	public:
		ThreadObject(FPTR f):fptr(f),assignment(NULL),isHealthy(0){}
		int isActive()const;
		void isActive(int x);
		static void* thread_func(void* arg);
		int Activate();
		void waitForWork();
		pthread_t getThreadId()const;
		int getHealth()const
		{
			return isHealthy;
		}
};


#endif
