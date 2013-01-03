/**
 * @File     : GradNode.cxx
 * @Authors  : L. Di-Jorio
 * @Date     : 29/10/2007
 * @Version  : V1.0
 * @Synopsis : PSP structure implementation
**/

#include "DBMain.h"
#include "Sort.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <vector>

using namespace std;
using namespace nsPattern;

#define DBMAIN nsPattern::DBMain
#define NULLVAL 0

DBMAIN::DBMain (void) throw ()
{
	m_Customer = 0;
	m_Attribute = 0;
	m_Base = NULL;
}
/**
 * Dynamic allocation of the database
 * Each element are reserved
 */
DBMAIN::DBMain (const int & Cust, const int & Attr) throw ()
{
	m_Customer = Cust;
	m_Attribute = Attr;
	m_Base = new int * [Attr];
	for (int i = 0; i < Attr; i++)
		m_Base[i] = new int [Cust];
}

DBMAIN::DBMain (int ** Base, const int & Cust, const int & Attr) throw ()
{
	m_Customer = Cust;
	m_Attribute = Attr;

	m_Base = new int * [m_Attribute];
	for (int i = 0; i < m_Attribute; i++)
	{
		m_Base[i] = new int [m_Customer];
		for (int j = 0; j < m_Customer; j++)
		{
			m_Base [i][j] = Base[i][j];
		}
	}
}

DBMAIN::DBMain (nsPattern::DBMain * db) throw ()
{
	m_Customer = db->GetCustomer ();
	m_Attribute = db->GetAttribute ();

	m_Base = new int * [m_Attribute];
	for (int i = 0; i < m_Attribute; i++)
	{
		m_Base[i] = new int [m_Customer];
		for (int j = 0; j < m_Customer; j++)
			m_Base[i][j] = db->GetValue (j, i);
	}

}

/**
 * First desallocate the second dimension
 * Then, delete main array
 */
DBMAIN::~DBMain (void) throw ()
{
	for (int i = 0; i < m_Attribute; i++)
	{
		delete [] m_Base[i];
		//m_Base[i] = NULL;
	}
	delete [] m_Base;
	m_Base = NULL;
}

void DBMAIN::Initialize (const int & Cust, const int & Attr) throw ()
{
	 m_Base = new int * [Attr];

	 for (int i = 0; i < Attr; i++)
		 m_Base[i] = new int [Cust];

	 m_Customer = Cust;
	 m_Attribute = Attr;
}

bool compare (nsPattern::Sort s1, nsPattern::Sort s2)
{
	if (s1.a1 == s2.a1)
		return (s1.a2 < s2.a2);
	return s1.a1 < s2.a1;
}

/**
 * TODO Bien commenter cette fabuleuse fonction
 * Generer l'ensemble des conflit pour un 2-itemset
 * Optimisation indispensable pour repasser d'un sort
 * à un DBMain classique...
 */
DBMAIN * DBMAIN::TwoGeneration (const int & At1, const int & At2) throw ()
{
	vector <nsPattern::Sort> Tab;

	for (int i = 0; i < m_Customer; i++)
	{
		if (m_Base[At1][i] != NULLVAL && m_Base[At2][i] != NULLVAL)
		{
			nsPattern::Sort s (m_Base[At1][i], m_Base[At2][i], m_Base[0][i]);
			Tab.push_back (s);
		}
	}

	//Trier (Tab, m_Customer);
	if (Tab.size () > 1)
	{
		std::stable_sort (Tab.begin (), Tab.end (), compare);

		int VAt1 [/*m_Customer*/Tab.size ()];
		int VAt2 [/*m_Customer*/Tab.size ()];
		int cl   [/*m_Customer*/Tab.size ()];

		for (unsigned int i = 0; i < /*m_Customer*/Tab.size (); i++)
		{
			cl   [i] = Tab[i].GetC ();
			VAt1 [i] = Tab[i].GetA1 ();
			VAt2 [i] = Tab[i].GetA2 ();
		}

		DBMAIN * result = new DBMAIN (/*m_Customer*/Tab.size (), 2);
		result->SetAttributeVal(0, cl);
		result->SetAttributeVal(1, VAt2);

		return result;
	}
	else
		return NULL;
}

/**
 * Update an item value list
 */
void DBMAIN::SetAttributeVal (const int & Attr, int * value) throw ()
{
	for (int i = 0; i < m_Customer; i++)
		m_Base[Attr][i] = value[i];
}

void DBMAIN::PrintInfo (void) throw ()
{
	for (int i = 0; i < m_Customer; i++)
	{
		for (int j = 0; j < m_Attribute; j++)
			cout << setw (6) << m_Base[j][i];
		cout << endl;
	}
}

void DBMAIN::Resize (int & newSize, const int & Attr) throw ()
{
	int ** test = new int * [m_Attribute];

	for (int i = 0; i < m_Attribute; i++)
	{
		test[i] = new int [m_Customer];
		for (int j = 0; j < m_Customer; j++)
		{
			test[i][j] = m_Base[i][j];
		}

		delete [] m_Base [i];
		m_Base[i] = new int [newSize];
	}

	int cpt = 0;
	for (int j = 0; j < m_Customer; j++)
	{
		if (test [Attr][j] != -1)
		{
			for (int i = 0; i < m_Attribute; i++)
			{
				m_Base[i][cpt] = test [i][j];
			}
			cpt++;
		}
	}
	m_Customer = newSize;

	for (int i = 0; i < m_Attribute; i++)
	{
		delete [] test [i];
		test[i] = NULL;
	}
	delete [] test;
	test = NULL;
}

void DBMAIN::Resize (int & newSize) throw ()
{
	int ** test = new int * [m_Attribute];

	for (int i = 0; i < m_Attribute; i++)
	{
		test[i] = new int [newSize];
		for (int j = 0; j < newSize; j++)
		{
			test[i][j] = m_Base[i][j];
		}

		delete [] m_Base [i];
		m_Base[i] = new int [newSize];
		m_Base[i] = test[i];
	}

	m_Customer = newSize;
}

vector <int> DBMAIN::GetAllCustomer (void) throw ()
{
	vector <int> result (m_Customer);
	for (int i = 0; i < m_Customer; i++)
		result.push_back (m_Base[0][i]);
	return result;
}

double DBMAIN::GetVariation (void) throw ()
{
	double result = 0.0;

	int egual = 0;
	for (int i = 1; i < m_Customer ; i++)
	{
		if (m_Base [m_Attribute -1][i] == m_Base[m_Attribute -1][i-1])
		{
			egual++;
		}
	}

	if (egual == m_Customer-1)
		egual++;
	result = (double) (egual) / (double) m_Customer;
	return (1.0 - result);
}

ostream& nsPattern::operator<< (ostream & os, DBMAIN * db) throw ()
{
	for (int i = 0; i < db->GetAttribute () ; i++)
	{
		for (int j = 0; j < db->GetCustomer (); j++)
			os << setw (6) << db->GetValue (j, i);
		os << endl;
	}
	return os;
}

int ** DBMAIN::GetBase (void) throw ()
{
	return m_Base;
}

void DBMAIN::SetBase (int ** Base) throw ()
{
	m_Base = Base;
}

int DBMAIN::GetValue (const int & Client,const int & Attr) throw ()
{
	return m_Base[Attr][Client];
}

void DBMAIN::SetValue (const int & Client, const int & Attr, const int & Val) throw ()
{
	m_Base[Attr][Client] = Val;
}

int DBMAIN::GetCustomer (void) throw ()
{
	return m_Customer;
}

void DBMAIN::SetCustomer (int Customer) throw ()
{
	m_Customer = Customer;
}

int DBMAIN::GetAttribute (void) throw ()
{
	return m_Attribute;
}

void DBMAIN::SetAttribute (int Attribute) throw ()
{
	m_Attribute = Attribute;
}

int * DBMAIN::GetDimension (const int & Attr) throw ()
{
	return m_Base [Attr];
}
#undef DBMAIN
