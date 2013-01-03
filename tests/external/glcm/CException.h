/**
 *
 * @File : CException.h
 *
 * @Authors : M. Laporte, D. Mathieu
 *
 * @Date : 03/08/2001
 *
 * @Version : V1.0
 *
 * @Synopsis : d�clarations des classes CException et CExcFct
 *
 **/
#if !defined __CEXCEPTION_H__
#define      __CEXCEPTION_H__

#include <exception>
#include <string>
#include <iostream>

namespace nsUtil
{

	class CException: public std::exception
	{
	public:
		std::string message;
		int CodeError;
		CException( std::string m, int c );
		virtual ~ CException(void) throw ();
		void Report();
	}; // CException


} // nsUtil

#endif /* __CEXCEPTION_H__ */
