#if !defined __ITEMSET_H__
#define      __ITEMSET_H__

#include <vector>
#include <algorithm>

#include "CException.h"

using namespace std;

namespace nsPattern
{
    class ItemSet
    {
		protected :
			vector<int> m_ItemSet;
			int _core_i;

		public :
            ItemSet (void) throw ();
			ItemSet (int item) throw ();
			ItemSet (vector<int> & vs) throw ();
			ItemSet (vector<int> & vs, int Core_i) throw ();
			virtual ~ItemSet (void) throw ();


			vector<int> getItemList (void) throw ();
			void setItemList (vector<int> & vs) throw ();


			/***************************************/
			/*		Methods		       */
			/***************************************/
			int getItemAt (int index) throw (nsUtil::CException);
			int  size(void) throw ();
			void clear() throw ();

			void addItem (int item) throw ();
			void deleteItemBack (void) throw ();
			void deleteItem (int item) throw ();

			bool contains(int item) throw ();
			bool contains(ItemSet & item) throw ();

			void Reset(void) throw ();
			//void Afficher (void) throw ();


			//NEW
			void Normalize (void) throw ();
			int getCore_i() throw();
			void setCore_i(int Core_i) throw();
			ItemSet addItemwCore_i (int item) throw ();
			void deleteItemwCore_i (int item) throw ();
			bool prefixEqual(ItemSet IS) throw ();
			int getItemIndex(int item) throw ();

			bool operator== (ItemSet & is) throw ();
			bool operator!= (ItemSet & is) throw ();

    }; // ItemSet
    std::ostream& operator<< (std::ostream & os, nsPattern::ItemSet & a) throw ();

    inline ItemSet::ItemSet (void) throw () {}
    
    inline ItemSet::ItemSet (int item) throw ()
    {
        m_ItemSet.push_back(item);
    }
    
    inline ItemSet::ItemSet (vector<int> & vs) throw ()
    {
        m_ItemSet = vs;
        _core_i = -2;
    }
    
    inline ItemSet::ItemSet (vector<int> & vs, int Core_i) throw ()
    {
        m_ItemSet = vs;
        _core_i = Core_i;
    }

    inline ItemSet::~ItemSet (void) throw () {}
    
    inline vector<int> ItemSet::getItemList (void) throw ()
    {
        return m_ItemSet;
    }
    inline void ItemSet::setItemList (vector<int> & vs) throw ()
    {
        m_ItemSet = vs;
    }
    
    /***************************************/
    /*		Methods			   */
    /***************************************/
    
    inline int ItemSet::getItemAt (int index) throw (nsUtil::CException)
    {
        if ((unsigned)index >= m_ItemSet.size ())
            throw (nsUtil::CException ("Out of bounds", 136));
        else
        {
            return m_ItemSet[index];
        }
    }
    
    inline int ItemSet::size() throw ()
    {
        return m_ItemSet.size ();
    }
    
    inline void ItemSet::clear() throw ()
    {
        m_ItemSet.clear ();
    }
    
    inline void ItemSet::addItem (int item) throw ()
    {
        m_ItemSet.push_back (item);
    }
    
    inline void ItemSet::deleteItemBack (void) throw ()
    {
        m_ItemSet.pop_back();
    }

    inline bool ItemSet::contains (int item) throw ()
    {
        for (int i = 0; i < (signed)m_ItemSet.size(); i++)
    	if (m_ItemSet[i] == item)
    	    return true;
        return false;
    }

    inline ItemSet ItemSet::addItemwCore_i (int item) throw ()
    {
    	if (item %2 == 0)
    	{
    		if (contains(item) || contains(item + 1)) return ItemSet(m_ItemSet,_core_i);
    	}
    	else if (contains(item) || contains(item - 1)) return ItemSet(m_ItemSet,_core_i);
        vector <int> tvIS = m_ItemSet;//.push_back (item);
        tvIS.push_back(item);
        int core_i = item;
    	ItemSet is = ItemSet(tvIS,core_i);
    	//m_ItemSet.pop_back();
    	is.Normalize();
        return is;
    }

    inline void ItemSet::Normalize (void) throw ()
    {
    	sort(m_ItemSet.begin(),m_ItemSet.end());
    }

    inline void ItemSet::deleteItemwCore_i (int item) throw ()
    {
    	 vector<int>::iterator iter = m_ItemSet.begin ();
    	 for (iter = m_ItemSet.begin (); iter != m_ItemSet.end (); iter++)
    		 if (*iter == item)
    		 {
    			 m_ItemSet.erase (iter);
    			 _core_i = *(iter - 1);
    		 }
    }

    inline int ItemSet::getCore_i() throw()
    {
    	return _core_i;
    }

    inline void ItemSet::setCore_i(int Core_i) throw()
    {
    	_core_i = Core_i;
    }

    inline bool ItemSet::prefixEqual(ItemSet IS) throw ()
    {
    	if (_core_i >= 0)
    	{
    		for (int i = 0; i <= IS.getItemIndex(IS.getCore_i()); i++)
    		{
    			if (m_ItemSet[i] != IS.getItemAt(i)) return false;
    		}
    	}
    	return true;
    }

    inline int ItemSet::getItemIndex(int item) throw ()
    {
    	if (contains(item))
    	{
    		for (int i = 0; i < (signed)m_ItemSet.size(); i++)
    		{
    			if (m_ItemSet[i] == item) return i;
    		}
    	}
    	return -1;
    }
} //nsPattern

#endif /* __ITEMSET_H__ */
