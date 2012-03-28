#include "ItemSet.h"

using namespace std;
using namespace nsPattern;

#define ITEMSET nsPattern::ItemSet

void ITEMSET::deleteItem (int item) throw ()
{
	 vector<int>::iterator iter = m_ItemSet.begin ();
	 for (iter = m_ItemSet.begin (); iter != m_ItemSet.end (); iter++)
		 if (*iter == item)
			 m_ItemSet.erase (iter);
}


ostream& nsPattern::operator<< (ostream & os, ITEMSET & a) throw ()
{
    os << '(';
    vector<int> vs = a.getItemList();
    for (int i = 0; i < (signed)vs.size(); i++)
    {
		if (i == 0)
			os << vs[i];
		else os << ' ' << vs[i];
    }
    os << ')';
    return os;
}

bool ITEMSET::operator== (ITEMSET & is) throw ()
{
	if (m_ItemSet.size () != (unsigned)is.size ())
		return false;

	vector<int>::iterator it1, it2;
	vector<int> vIS = is.getItemList();
	it2 = m_ItemSet.begin ();

	for (it1 = vIS.begin (); it1 != vIS.end (); it1++)
	{
		if (*it1 != *it2)
			return false;
		it2++;
	}

	return true;
}

bool ITEMSET::operator!= (ITEMSET & is) throw ()
{
    /*m_ItemSet.sort ();
    m_ItemSet.unique ();

    is.Normalize ();*/
	if (m_ItemSet.size () != (unsigned)is.size ())
		return true;

	vector<int>::iterator it1, it2;
	vector<int> is1 = is.getItemList ();
	it2 = m_ItemSet.begin ();

	for (it1 = is1.begin (); it1 != is1.end (); it1++)
	{
		if (*it1 != *it2)
			return true;
		it2++;
	}

	return false;
}

#undef ITEMSET
