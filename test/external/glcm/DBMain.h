#if !defined __DBMAIN_H__
#define      __DBMAIN_H__

#include "CException.h"

#include <vector>

namespace nsPattern
{
    class DBMain
    {
		protected :
			int ** m_Base;
			int m_Customer;
			int m_Attribute;

		public :
			DBMain (void) throw ();
			DBMain (const int & Cust, const int & Attr) throw ();
			DBMain (int ** Base, const int & Cust, const int & Attr) throw ();
			DBMain (nsPattern::DBMain * db) throw ();
			virtual ~DBMain (void) throw ();

			int **	GetBase			(void) throw ();
			void	SetBase			(int ** Base) throw ();
			int		GetValue		(const int & Client, const int & Attr) throw ();
			void	SetValue		(const int & Client, const int & Attr, const int & Val) throw ();
			int		GetCustomer		(void) throw ();
			void	SetCustomer		(int Customer) throw ();
			int		GetAttribute	(void) throw ();
			void	SetAttribute	(int Attribute)	throw ();
			void	SetAttributeVal (const int & Attr, int * value) throw ();
			int *	GetDimension	(const int & Attr) throw ();

			/***************************************/
			/*				Methods				   */
			/***************************************/

			void Initialize (const int & Cust, const int & Attr) throw ();
			void PrintInfo (void) throw ();
			void EraseAttribute (const int & Attribute) throw ();
			void EraseCustomer  (const int & Customer) throw ();
			void SortByLast (void) throw ();
			nsPattern::DBMain * TwoGeneration (const int & At1, const int & At2) throw ();
			void Resize (int & newSize, const int & Attr) throw ();
			void Resize (int & newSize) throw ();
			std::vector <int> GetAllCustomer (void) throw ();
			double GetVariation (void) throw ();

    }; // DBMain

    std::ostream& operator<< (std::ostream & os, nsPattern::DBMain * db) throw ();
} //nsPattern

#endif /* __DBMAIN__H__ */
