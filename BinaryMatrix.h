#if !defined __BINARYMATRIX_H__
#define      __BINARYMATRIX_H__

#include "CException.h"
#include "TransactionSequence.h"

#include <stdint.h>
#include <vector>
#include <list>
#include <string.h>



typedef pair<int, int> trans_t;
typedef vector<trans_t> id_trans_t; 

namespace nsPattern
{
    class BinaryMatrix
    {
		protected :
			uint32_t * m_BimaryMatrix;
			int m_size;

		public :
			BinaryMatrix (void) throw ();
			BinaryMatrix (const int size) throw ();
			BinaryMatrix (uint32_t * binaryMatrix, const int size) throw ();
			BinaryMatrix (BinaryMatrix * bm) throw ();
            BinaryMatrix (const BinaryMatrix& bm) throw ();
			virtual ~BinaryMatrix (void) throw ();


			/***************************************/
			/*				Methods				   */
			/***************************************/

            BinaryMatrix& operator=(const BinaryMatrix& bm);
        
			uint32_t *	getBimaryMatrix	(void) throw ();
			void	setBimaryMatrix	(uint32_t * binaryMatrix) throw ();
			int		getSize			(void) const throw ();
			void	setSize			(int size) throw ();

			bool getValue (const int column, const int row) throw ();
			void setValue (const int column, const int row, const bool & Val) throw ();
			void Initialize (const int size)  throw ();
			void PrintInfo (void) throw ();
			uint32_t * GetDimension (const int & size) throw ();

			//get count matrix where equal to 1
			int getActive() throw ();//Frequency count

			//Normal Frequency
			int getMaxTrans(int & maxCount) throw ();
			bool checkRootTree(int k);
			int getMaxChild(int transaction) throw ();
			bool checkZero(int transaction) throw ();


			//set to 0
			void setTo0(int k) throw ();

			//Check one Matrix included
			bool isInclude(BinaryMatrix * bm) throw ();
        
            BinaryMatrix& operator&= (BinaryMatrix& bm) throw ();

			//
			void constructBinaryMatrix(int item, vector<TransactionSequence> & G1Item);
			void constructBinaryMatrixClogen(const id_trans_t &transaction, std::vector<std::vector <int> > *sibling, int nb_trans);
			//to write to file
			list<pair<int,int> > findPair() throw();
			list < pair<int,int> > buildLeastOrderMap() throw ();

			vector <int> buildLeastOrder_0() throw ();
			vector <int> buildGreatOrder_0() throw ();
			vector <int> findAllChildren(int transaction) throw ();
			vector <int> findAllDirectChildren(int transaction) throw ();

			//BinaryMatrix * operator& (BinaryMatrix * bm) throw ();
			bool operator== (BinaryMatrix & bm) throw ();


			//For test
			int count_TS_Support(int transaction);
			void reduce_nonSupport_TS(int threshold, bool & change);
			bool checkTSZero(int k);
			void setBMtoZero();
			bool checkBMZero();

    }; // BINARYMATRIX

    inline BinaryMatrix& BinaryMatrix::operator=(const BinaryMatrix& bm) {
        if (m_size < bm.m_size) {
            delete [] m_BimaryMatrix;
            m_BimaryMatrix = new uint32_t [((bm.m_size>>5)+1)*bm.m_size];
        }
        m_size = bm.m_size;
        memcpy(m_BimaryMatrix, bm.m_BimaryMatrix, ((bm.m_size>>5)+1)*bm.m_size*sizeof(uint32_t));
        return *this;
    }

    inline uint32_t * BinaryMatrix::getBimaryMatrix (void) throw ()
    {
        return m_BimaryMatrix;
    }
    
    inline void BinaryMatrix::setBimaryMatrix (uint32_t * binaryMatrix) throw ()
    {
        m_BimaryMatrix = binaryMatrix;
    }
    
    inline bool BinaryMatrix::getValue (const int column, const int row) throw ()
    {
        return (m_BimaryMatrix[row*((m_size>>5)+1)+(column>>5)] & (1 << column%32));
//        return m_BimaryMatrix[row*m_size+column];
    }
    
    inline void BinaryMatrix::setValue (const int column, const int row, const bool & Val) throw ()
    {
        if (Val)
            m_BimaryMatrix[row*((m_size>>5)+1)+(column>>5)] |= (1 << column%32);
        else
            m_BimaryMatrix[row*((m_size>>5)+1)+(column>>5)] &= ~(1 << column%32);
        //m_BimaryMatrix[row*m_size+column] = Val;
        //	 m_BimaryMatrix[row][column] = Val;
    }
    
    inline int BinaryMatrix::getSize (void) const throw ()
    {
        return m_size;
    }
    
    inline void BinaryMatrix::setSize (int size) throw ()
    {
        m_size = size;
    }
    
    inline uint32_t * BinaryMatrix::GetDimension (const int & size) throw ()
    {
        return m_BimaryMatrix + size*m_size;
        //	return m_BimaryMatrix [size];
    }
    
    inline BinaryMatrix& BinaryMatrix::operator&= (BinaryMatrix& bm) throw ()
    {
	int BMSize = ((bm.m_size>>5)+1)*bm.m_size;
        for (int i = 0; i < BMSize; i++)
            m_BimaryMatrix[i] &= bm.m_BimaryMatrix[i];

//        for (int i = 0; i < m_size*m_size; i++)
//            m_BimaryMatrix[i] = (m_BimaryMatrix[i] && bm.m_BimaryMatrix[i]);
        return *this;
    }



//For test
	inline int BinaryMatrix::count_TS_Support(int k)
	{
		int count = 0;
		for (int i = 0 ; i < m_size; i++)
		{
//        this->setValue(k,i,0);
//        this->setValue(i,k,0);
			if ((m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] & (1 << k%32))) count++; 
 			if ((m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] & (1 << i%32))) count++; 
			//m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] &= ~(1 << k%32);
        		//m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] &= ~(1 << i%32);
		}
		return count;
	}

	inline void BinaryMatrix::setTo0(int k) throw ()
	{
		for (int i = 0 ; i < m_size; i++)
		{
	//        this->setValue(k,i,0);
	//        this->setValue(i,k,0);
	        m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] &= ~(1 << k%32);
	        m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] &= ~(1 << i%32);
		}
	}

	inline bool BinaryMatrix::checkTSZero(int k)
	{
		for (int i = 0 ; i < m_size; i++)
		{
			if ((m_BimaryMatrix[i*((m_size>>5)+1)+(k>>5)] & (1 << k%32))) return false; 
 			if ((m_BimaryMatrix[k*((m_size>>5)+1)+(i>>5)] & (1 << i%32))) return false;
		}
		return true;
	}

	inline void BinaryMatrix::reduce_nonSupport_TS(int threshold, bool & change)
	{
		int count;
		//if (!(change)) return;
		change = false;
		int i;
		for (i = 0; i < m_size; i++)
		{
			count = count_TS_Support(i);
			if (count < (threshold-1)) 
			{
				if (checkTSZero(i)) change = false;			
				else
				{				
					setTo0(i);
					change = true;
					//cout << "iiiiiiii" << i << endl;
					break;
				}
			//	reduce_nonSupport_TS(threshold,i);
			}
			change = false;
		}
		//cout << "iiiiiiii" << i << endl;
		//if (i == m_size) change = false;
		//reduce_nonSupport_TS(threshold,change);
	}

	inline bool BinaryMatrix::checkBMZero()
	{
		for (int i = 0 ; i < m_size; i++)
		{	
			for (int j = 0 ; j < m_size; j++)
			{
				if ((m_BimaryMatrix[i*((m_size>>5)+1)+(j>>5)] & (1 << j%32))) return false; 
 				if ((m_BimaryMatrix[j*((m_size>>5)+1)+(i>>5)] & (1 << i%32))) return false;
			}
		}
		return true;
	}
	

	inline void BinaryMatrix::setBMtoZero()
	{
		bzero(m_BimaryMatrix, ((m_size>>5)+1)*m_size*sizeof(uint32_t));
	}
    std::ostream& operator<< (std::ostream & os, nsPattern::BinaryMatrix * db) throw ();
} //nsPattern

#endif /* __BINARYMATRIX__H__ */
