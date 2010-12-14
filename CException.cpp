/*
 *
 * CException.cpp
 *
 */

#if !defined __CEXCEPTION_CPP__
#define      __CEXCEPTION_CPP__

#include <exception>
#include <string>
#include <iostream>

#include "CException.h"

using namespace std;

nsUtil::CException::CException( std::string m, int c ) { message = m; CodeError = c; };
nsUtil::CException::~ CException(void) throw () {};

void nsUtil::CException::Report()
{
	cerr<< "Exception caught: " << message << " , "<< CodeError << endl;
}


#endif /* __CEXCEPTION_CPP__ */
