#ifndef WORKUNIT_H
#define WORKUNIT_H

#include <iostream>
#include <sstream>
#include <string>

struct workUnit		//choosing struct coz want access to variables via ptr. No point hiding them.
{
	double result;
	double S;	//Option Price
	double K;	//Strike Price
	double r;	//Risk Free Rate
	double V;	//Volatility
	double T;	//Maturity
	double loops;	//No. of times to run
	public:
	workUnit(double SS,double KK,double rr,double VV,double TT,double ll):
	S(SS),K(KK),r(rr),V(VV),T(TT),loops(ll),result(0.0)
	{}
//	double result;
};
#endif

