#include <stdio.h>
#include <algorithm>

#include "TransactionSequence.h"



using namespace nsPattern;
using namespace std;

#define TRANSACTIONSEQUENCE nsPattern::TransactionSequence

struct SortDe
{
    bool operator()(int a , int b)
	{
		return a > b;
	};
};

TRANSACTIONSEQUENCE::TransactionSequence (void) throw () {}

TRANSACTIONSEQUENCE::TransactionSequence (int transaction) throw ()
{
	m_TransactionSequence.push_back(transaction);
}

TRANSACTIONSEQUENCE::TransactionSequence (vector<int> vs) throw ()
{
	m_TransactionSequence = vs;
}

TRANSACTIONSEQUENCE::~TransactionSequence (void) throw () {}

/***************************************/
/*		Methods			   */
/***************************************/

int TRANSACTIONSEQUENCE::size() throw ()
{
    return m_TransactionSequence.size ();
}

void TRANSACTIONSEQUENCE::clear() throw ()
{
	m_TransactionSequence.clear ();
}

void TRANSACTIONSEQUENCE::sortSEQ(int INorDE) throw ()
{
	if (INorDE == 0) sort(m_TransactionSequence.begin(),m_TransactionSequence.end());
	else sort(m_TransactionSequence.begin(),m_TransactionSequence.end(),SortDe());
}

void TRANSACTIONSEQUENCE::reverseTransactionSeqOrder() throw ()
{
	vector<int> reverseVS;
	reverseVS.clear();
	vector<int>::iterator it;
	for (int i = m_TransactionSequence.size() - 1; i >= 0 ; i--){
		reverseVS.push_back(m_TransactionSequence[i]);
	}
	m_TransactionSequence = reverseVS;
}

void TRANSACTIONSEQUENCE::addTransactionBack (int transaction) throw ()
{
	m_TransactionSequence.push_back (transaction);
}

void TRANSACTIONSEQUENCE::deleteTransactionBack (void) throw ()
{
	m_TransactionSequence.pop_back ();
}

void TRANSACTIONSEQUENCE::deleteTransaction (int transaction) throw ()
{
    vector<int>::iterator iter;
    for (iter = m_TransactionSequence.begin (); iter != m_TransactionSequence.end (); iter++)
	if (*iter == transaction) iter = m_TransactionSequence.erase (iter);
}

int TRANSACTIONSEQUENCE::contains (int transaction) throw ()
{
    for (int i = 0; i < (signed)m_TransactionSequence.size(); i++)
    {
    	if (m_TransactionSequence[i] == transaction) return i;
    }
    return -1;
}

bool TRANSACTIONSEQUENCE::contains(TransactionSequence transactionSequence) throw ()
{
	int k = 0;
	int k1 = -1;
	for (int i = 0; i < transactionSequence.size(); i++)
	{
		int k2 = contains(transactionSequence.getTransactionAt(i));
		if ((k2 != -1) && (k2 < k1)) return false;
		else if ((k2 != -1) && (k2 > k1)) k++;
		k1 = k2;
	}

	if (k == transactionSequence.size())
		return true;
	else return false;
}

int TRANSACTIONSEQUENCE::getTransactionAt (int index) throw (nsUtil::CException)
{
    if ((unsigned)index >= m_TransactionSequence.size ())
	throw (nsUtil::CException ("Out of bounds", 136));
    else
    {
		return m_TransactionSequence[index];
    }
}

vector<int> TRANSACTIONSEQUENCE::getTransactionVector() throw ()
{
	return m_TransactionSequence;
}

ostream& nsPattern::operator<< (ostream & os, TRANSACTIONSEQUENCE & a) throw ()
{
    os << '(';
    vector<int> vs = a.getTransactionVector ();
    for (int i = 0; i < (signed)vs.size(); i++)
    {
		if (i == 0)
			os << vs[i];
		else os << ' ' << vs[i];
    }
    os << ')';
    return os;
}

bool TRANSACTIONSEQUENCE::operator== (TRANSACTIONSEQUENCE & is) throw ()
{
    if (m_TransactionSequence.size () != (unsigned)is.size ())
	return false;

    vector<int>::iterator it1, it2;
    vector<int> vIS = is.getTransactionVector();
    it2 = m_TransactionSequence.begin ();

    for (it1 = vIS.begin (); it1 != vIS.end (); it1++)
    {
	if (*it1 != *it2)
	    return false;
	it2++;
    }

    return true;
}
/*
bool TRANSACTIONSEQUENCE::operator!= (TRANSACTIONSEQUENCE & is) throw ()
{
    m_TransactionList.sort ();
    m_TransactionList.unique ();

    is.Normalize ();
    if (m_TransactionList.size () != (unsigned)is.GetSize ())
	return true;

    list<int>::iterator it1, it2;
    list<int> is1 = is.GetTransactionList ();
    it2 = m_TransactionList.begin ();

    for (it1 = is1.begin (); it1 != is1.end (); it1++)
    {
		if (*it1 != *it2)
			return true;
		it2++;
    }

    return false;
}*/

#undef TRANSACTIONSEQUENCE
