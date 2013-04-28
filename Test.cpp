#include<iostream>
#include<fstream>
#include<algorithm>	//For max function used in run method.
#include<cmath>
#include "WorkUnit.h"	//This is a file written to test the library
#include "THREAD_POOL/thread_pool.h" 
#include<new>
extern "C" {
#include<unistd.h>
#include<stdlib.h>
}
#include<cstdio>

using namespace std;

double gaussian_box_muller() 
{
  double x = 0.0;
  double y = 0.0;
  double euclid_sq = 0.0;

  // Continue generating two uniform random variables
  // until the square of their "euclidean distance" 
  // is less than unity
  do {
    x = 2.0 * rand() / static_cast<double>(RAND_MAX)-1;
    y = 2.0 * rand() / static_cast<double>(RAND_MAX)-1;
    euclid_sq = x*x + y*y;
  } while (euclid_sq >= 1.0);

  return x*sqrt(-2*log(euclid_sq)/euclid_sq);
}


void* run(void *arg) 	//This is the function the threads of the thread pool will run
{
	workUnit* ptr=static_cast<workUnit*>(arg);
	double S_adjust=(ptr->S)*exp((ptr->T)*((ptr->r)-0.5*(ptr->V)*(ptr->V)));
	double S_cur = 0.0;

	for(int i=0; i<ptr->loops; ++i)	//Running the simulation loops number of times.
	{
		double gauss_bm = gaussian_box_muller();
		S_cur = S_adjust * exp(sqrt((ptr->V)*(ptr->V)*(ptr->T))*gauss_bm);
		ptr->result += max(S_cur-(ptr->K),0.0);			
	}
        return arg;
}

double finalresult=0.0;


void* getResults(void *arg)	//This is the function which will collect results from the thread pool
{				//and do the neccessary summations
	double summation=0.0;
	double count=0;
	double r,T;
	int once=0;
        void *ptr=NULL;
	workUnit* res;
        while(1)
        {	
                ptr=result();  //Using the function provided by the thread pool library to collect result
		if(ptr==NULL)
		{
			break;
		}
		res=static_cast<workUnit*>(ptr);
		summation+=res->result;
		count+=res->loops;
		if(!once)
		{
			r=res->r;
			T=res->T;
		}
		delete res;
        }
	finalresult=(summation/static_cast<double>(count))*exp((-r)*T);
        pthread_exit(NULL);
	return NULL;
}

int main()
{
	cout<<"\n\nThis program calculates the option call price using the thread pool library for the sake of illustration of the thread pool library. The pricing factors using which the option put price will be calculated are hardcoded in the code as this is just a demo program.\n";
	int numOfThreads;
	cout<<"\nEnter the number of threads to be created by thread pool library : ";
	cin>>numOfThreads;
	thread_pool pool(numOfThreads,run); //Letting library know the number of threads and function
	try
	{
		pool.startPool();	//Start the threads.
	}catch(const runtime_error& err)
		{
                        cout<<err.what();
                        return 0;
                }
			
	pthread_t resultID;
	pthread_create(&resultID, NULL, getResults, NULL);

 	 // First we create the parameter list                                                                               
  	int num_sims;   // Number of simulated asset paths                                                       
	cout<<"\nEnter the number of simulations (Keep it at least 10000000 for accurate results) : ";
	cin>> num_sims;
	int numOfWorkObjects=num_sims/10000; 	

	for(int i=0; i<numOfWorkObjects; ++i)
	{
		try
		{
		workUnit* ptr=new  workUnit(100.0,100.0,0.05,0.2,1.0,10000); 
		queue(static_cast<void*>(ptr));
		}catch(const bad_alloc& e)
			{
				cout<<"\nUnable to allocate memory.\n";
				pool.shutDownPool();    //Shut down of pool started
				pthread_join(resultID, NULL);   //Clean up the thread running getResults function

				return 0;
			}
	}
	pool.shutDownPool();	//Shut down of pool started
	pthread_join(resultID, NULL);	//Clean up the thread running getResults function
	cout<<"\n The call price is:"<<finalresult<<"\n";
	return 1;
}
