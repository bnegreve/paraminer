#if !defined __TRANSACTIONSEQUENCE_H__
#define      __TRANSACTIONSEQUENCE_H__

#include <vector>
#include "CException.h"
using namespace std;

namespace nsPattern
{
    class TransactionSequence
    {
		protected :
			vector<int> m_TransactionSequence;

		public :
            TransactionSequence (void) throw ();
			TransactionSequence (int transaction) throw ();
			TransactionSequence (vector<int> vs) throw ();
			virtual ~TransactionSequence (void) throw ();


			/***************************************/
			/*		Methods		       */
			/***************************************/

			int  size(void) throw ();
			void clear(void) throw ();

			//Sort the TransactionSequence INorDE = 0: Increase; 1: Decrease
			void sortSEQ(int INorDE) throw ();

			//Reverse the order of a TransactionSequence
			void reverseTransactionSeqOrder() throw ();

			void addTransactionBack (int transaction) throw ();
			void deleteTransactionBack (void) throw ();
			void deleteTransaction (int transaction) throw ();

			int contains(int transaction) throw ();
			bool contains(TransactionSequence transactionSequence) throw ();

			int getTransactionAt (int index) throw (nsUtil::CException);

			vector<int> getTransactionVector() throw ();
			bool operator== (TransactionSequence& is) throw ();
			bool operator!= (TransactionSequence& is) throw ();

    }; // TransactionSequence
    std::ostream& operator<< (std::ostream & os, nsPattern::TransactionSequence& a) throw ();
} //nsPattern

#endif /* __TRANSACTIONSEQUENCE_H__ */
