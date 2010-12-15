
#include "BinaryMatrix.h"
#include "Sort.h"
#include "TransactionSequence.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <list>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <map>

using namespace std;
using namespace nsPattern;

#define BINARYMATRIX nsPattern::BinaryMatrix
#define NULLVAL 0

BINARYMATRIX::BinaryMatrix (void) throw ()
{
	m_size = 0;
	m_BimaryMatrix = NULL;
}

BINARYMATRIX::BinaryMatrix (const int size) throw ()
{
	if (size == 0)
	{
		m_size = 0;
		m_BimaryMatrix = NULL;
	}
	else
	{
		m_size = size;
		m_BimaryMatrix = new uint32_t [((m_size>>5)+1)*m_size];
		bzero(m_BimaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
	}
}

BINARYMATRIX::BinaryMatrix (uint32_t * binaryMatrix, const int size) throw ()
{
	m_size = size;
	m_BimaryMatrix = new uint32_t [((m_size>>5)+1)*m_size];
    memcpy(m_BimaryMatrix, binaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
}

BINARYMATRIX::BinaryMatrix (BinaryMatrix * bm) throw ()
{
    if (bm == NULL) {
        m_size = 0;
        m_BimaryMatrix = NULL;
    } else {
        m_size = bm->getSize();
        m_BimaryMatrix = new uint32_t [((m_size>>5)+1)*m_size];
        memcpy(m_BimaryMatrix, bm->m_BimaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
    }
}

BINARYMATRIX::BinaryMatrix (const BinaryMatrix& bm) throw ()
{
	m_size = bm.getSize();
	m_BimaryMatrix = new uint32_t [((m_size>>5)+1)*m_size];
    memcpy(m_BimaryMatrix, bm.m_BimaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
}

BINARYMATRIX::~BinaryMatrix (void) throw ()
{
	delete [] m_BimaryMatrix;
	m_BimaryMatrix = NULL;
}

void BINARYMATRIX::Initialize (const int size) throw ()
{
	m_size = size;
    bzero(m_BimaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
}

void BINARYMATRIX::PrintInfo (void) throw ()
{
	for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
			cout << setw (6) << this->getValue(j,i); //m_BimaryMatrix[i*m_size+j];
		cout << endl;
	}
}

bool BINARYMATRIX::operator== (BinaryMatrix & bm) throw ()
{
	for (int i = 0; i < ((m_size>>5)+1)*m_size; i++)
        if (m_BimaryMatrix[i] != bm.m_BimaryMatrix[i])
            return false;
	return true;
}

ostream& nsPattern::operator<< (ostream & os, BINARYMATRIX * db) throw ()
{
	for (int i = 0; i < db->getSize(); i++)
	{
		for (int j = 0; j < db->getSize(); j++)
			os << setw (6) << db->getValue(j, i);
		os << endl;
	}
	return os;
}

int BINARYMATRIX::getActive() throw ()
{
	int count = 0;
    for (int i = 0; i < ((m_size>>5)+1)*m_size; i++) {
    	register unsigned int v = m_BimaryMatrix[i];//register
        v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
        unsigned int c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
        count = count + c;
    }
	    	//std::bitset<sizeof(uint32_t) * 8> binary(v);
        //cout << "v" << v << endl;
        //v = ( v >> 16             ) | ( v               << 16);
        //cout << "v after" << v << endl;

//	for (int i = 0; i < m_size*m_size; i++)
//        count += (m_BimaryMatrix[i]);
	return count;
}

int BINARYMATRIX::getMaxTrans(int & maxCount) throw ()
{
	maxCount = 0;
	int maxTrans = -1;
	int mSizeDec = ((m_size>>5)+1);
	for (int i = 0; i < m_size * mSizeDec; i = i + mSizeDec)
	{
		int count = 0;
		for (int j = i; j < (mSizeDec + i); j++)
		{	
			register unsigned int v = m_BimaryMatrix[j];//register
        		v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
        		v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
        		unsigned int c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
			count += c;
		}
		if (count > maxCount)
		{
			maxCount = count;
			maxTrans = i/mSizeDec;
		}
		/*for (int j = 0; j < m_size; j++)
		{
			if (m_BimaryMatrix[i*mSizeDec+(j>>5)] & (1 << j%32)) //m_BimaryMatrix[i*m_size+j])  (this->getValue(j, i))
			{
				count++;
			}
		}
		if (count > maxCount)
		{
			maxCount = count;
			maxTrans = i;
		}*/
	}
	return maxTrans;
}

//max Child is a child that have the most value 1
int BINARYMATRIX::getMaxChild(int transaction) throw ()
{
	int maxCount = 0;
	int maxChild = -1;
	int mSizeDec = ((m_size>>5)+1);
	for (int i = 0; i < m_size; i++)
	{
		if  (m_BimaryMatrix[transaction*mSizeDec+(i>>5)] & (1 << i%32))//m_BimaryMatrix[transaction*m_size+i]) (this->getValue(i, transaction))
		{
			int child = i;
			//int c = 0;
			//child = i;

			//count where is 1 of a child
			/*for (int j = 0; j< m_size; j++)
			{
				if (m_BimaryMatrix[child*mSizeDec+(j>>5)] & (1 << j%32)) //m_BimaryMatrix[child*m_size+j]) (this->getValue(j, child))
                    c++;
			}*/
			int count = 0;
			for (int j = child * mSizeDec; j < (mSizeDec + child * mSizeDec); j++)
			{	
				register unsigned int v = m_BimaryMatrix[j];//register
        			v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
        			v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
        			unsigned int c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
				count += c;
			}
			if (count >= maxCount)
			{
				maxCount = count;
				maxChild = child;
			}
		}
	}
	return maxChild;
}

bool BINARYMATRIX::checkRootTree(int k)
{
	bool temp = false;
	for (int i = 0 ; i < m_size; i++)
	{
		if ((m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] & (1 << k%32))) return false;
		if ((m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] & (1 << i%32))) temp = true;
	}
	if (temp) return true;
	else return false;
}

/* returns true if transaction in bm contains nothing but 0 */
bool BINARYMATRIX::checkZero(int transaction) throw ()
{
	//cout << sizeof(uint32_t)<<endl;
	int mSizeDec = ((m_size>>5)+1);
    for (int i = (transaction * mSizeDec); i < (((transaction +1) * mSizeDec)); i++) {
    	if (m_BimaryMatrix[i])//if (this->getValue(i,transaction))//if (m_BimaryMatrix[i*((m_size>>5)+1)+(transaction>>5)])//
            return false;
    }
	//if (m_BimaryMatrix == NULL) return true;
	//if (m_BimaryMatrix[transaction])	return false;
	return true;
}

/*void BINARYMATRIX::setTo0(int k) throw ()
{
	for (int i = 0 ; i < m_size; i++)
	{
//        this->setValue(k,i,0);
//        this->setValue(i,k,0);
        m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] &= ~(1 << k%32);
        m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] &= ~(1 << i%32);
	}
}*/

bool BINARYMATRIX::isInclude(BinaryMatrix * bm) throw ()
{
	/*for (int i = 0; i < ((m_size>>5)+1)*m_size; i++)
	{
		int thisRow = bm->m_BimaryMatrix[i];
		int bmArow = (bm->m_BimaryMatrix[i] && m_BimaryMatrix[i]);
	    if (bmArow != thisRow) return false;
	}
		return true;*/

	//int bmSize = bm->getSize();
	int mSizeDec = ((m_size>>5)+1);

    for (int i = 0; i < m_size; i++)
	{
    	int iDec = (i>>5);
    	int iMod = (1 << i%32);
		for (int j = 0; j < m_size; j++)
            if ( (bm->m_BimaryMatrix[j*mSizeDec+iDec] & iMod) &&
                ( ( m_BimaryMatrix[i*mSizeDec+(j>>5)] ^ bm->m_BimaryMatrix[i*mSizeDec+(j>>5)] ) & (1 << j%32) ) )//!(m_BimaryMatrix[j*mSizeDec+iDec] & iMod))
                return false;
//            if ( bm->getValue(i,j) && (this->getValue(j,i) != bm->getValue(j,i)) )
//                return false;
	}
	return true;
}

void BINARYMATRIX::constructBinaryMatrix(int item, vector<TransactionSequence> & G1Item)
{
	TransactionSequence ts = G1Item[item];
	int tsSize = ts.size();
	for (int i = 0; i < tsSize; i++)
	{
		for (int j = 0; j < tsSize; j++)
		{
			int row = ts.getTransactionAt(i);
			int col = ts.getTransactionAt(j);
            /*if (j > i)
            m_BimaryMatrix[row*((m_size>>5)+1)+(col>>5)] |= (1 << col%32);
        else
            m_BimaryMatrix[row*((m_size>>5)+1)+(col>>5)] &= ~(1 << col%32);*/
			this->setValue(col, row, (j > i));
//            m_BimaryMatrix[row*m_size+col] = (j > i);
		}
	}
}


void  BINARYMATRIX::constructBinaryMatrixClogen(const id_trans_t &transaction)
{
  int col, row;
  for(int i = 0; i < transaction.size(); i++){
    col = transaction[i].first; 
    row = transaction[i].second;
    this->setValue(row, col, 1);
  }

//   }

// 	int tsSize = transaction.size();

	
// 	for (int i = 0; i < tsSize; i++)
// 	{
// 		for (int j = 0; j < tsSize; j++)
// 		{
// 			int row = ts.getTransactionAt(i);
// 			int col = ts.getTransactionAt(j);
//             /*if (j > i)
//             m_BimaryMatrix[row*((m_size>>5)+1)+(col>>5)] |= (1 << col%32);
//         else
//             m_BimaryMatrix[row*((m_size>>5)+1)+(col>>5)] &= ~(1 << col%32);*/
// 			this->setValue(col, row, (j > i));
// //            m_BimaryMatrix[row*m_size+col] = (j > i);
// 		}
// 	}
}

//to write to file
list<pair <int,int> > BINARYMATRIX::findPair() throw()
{
	list<pair <int,int> > lTSBM;
	lTSBM.clear();
	pair<int,int> pBM;//TransactionSequence tsBM;// = TransactionSequence();
	for (int i = 0; i < m_size; i++)
	{
		for (int j = 0; j < m_size; j++)
		{
			if(this->getValue(j,i))
			{
				/*tsBM.clear();
				tsBM.addTransactionBack(i);
				tsBM.addTransactionBack(j);*/
				pBM = make_pair(i,j);
				lTSBM.push_back(pBM);
			}
		}
	}
	return lTSBM;
}

list < pair<int,int> > BINARYMATRIX::buildLeastOrderMap() throw ()
{
	list < pair<int,int> > lMBM;
	lMBM.clear();
	pair<int,int> mBM;// = TransactionSequence();
	for (int i = 0; i < m_size; i++)
	{
		int count = 0;
		for (int j = 0; j < m_size; j++)
		{
			if(this->getValue(i,j))
			{
				count++;
			}
		}
		//cout << i << count << endl;
		mBM = make_pair (i,count);
		lMBM.push_back(mBM);
	}
	return lMBM;
}

//Node has no father
vector <int> BINARYMATRIX::buildLeastOrder_0() throw ()
{
	vector <int> vMBM;
	vMBM.clear();
	for (int i = 0; i < m_size; i++)
	{
		int count = 0;
		for (int j = 0; j < m_size; j++)
		{
			if(this->getValue(i,j))
			{
				count++;
			}
		}
		//cout << i << count << endl;
		if (count == 0)
		{
			cout << "LO" << i << endl;
			vMBM.push_back(i);
		}
	}
	return vMBM;
}

//Node has no child
vector <int> BINARYMATRIX::buildGreatOrder_0() throw ()
{
	vector <int> vMBM;
	vMBM.clear();
	for (int i = 0; i < m_size; i++)
	{
		if (m_BimaryMatrix[i] == 0)
			vMBM.push_back(i);
	}
	return vMBM;
}

//find all children of a transaction
vector <int> BINARYMATRIX::findAllChildren(int transaction) throw ()
{
	vector <int> tVInt;
	tVInt.clear();
	for (int i = 0; i< m_size; i++) {
		if (this->getValue(i,transaction))
			tVInt.push_back(i);
	}
	return tVInt;
}

//find all direct children of a transaction
vector <int> BINARYMATRIX::findAllDirectChildren(int transaction) throw ()
{
	vector <int> tVDInt;
	tVDInt.clear();
	vector <int> tVInt = findAllChildren(transaction);
	if (tVInt.size() == 0) return tVDInt;
	for (int i = 0; i < (signed)tVInt.size(); i++)
	{
		vector <int> tVCInt = findAllChildren(tVInt[i]);
		if ((tVInt.size() == (tVCInt.size() + 1)))
		{
			cout << " DC " << tVInt[i] << endl;
			tVDInt.push_back(tVInt[i]);
		}
	}
	return tVDInt;
}
#undef DBMAIN
