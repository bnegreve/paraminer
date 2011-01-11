/**
 * @File     : DBMan.h
 * @Authors  : L. Di-Jorio
 * @Date     : 29/10/2007
 * @Version  : V1.0
 * @Synopsis : A memory manager for a transactionnal database
**/

#if !defined __SORT_H__
#define      __SORT_H__

#include "CException.h"

namespace nsPattern
{
	class Sort
	{
		public :
			int a1;
			int a2;
			int c;

			Sort (int a, int b, int cl)
				: a1 (a), a2 (b), c(cl) {}
			~Sort (void) {}

			void SetA1 (int a) {a1 = a;}
			void SetA2 (int a) {a2 = a;}
			void SetC  (int cl) {c = cl;}
			const int  GetA1 (void) {return a1;}
			const int  GetA2 (void) {return a2;}
			int  GetC (void) {return c;}
	};
	
} //nsPattern

#endif /* __SORT__H__ */
