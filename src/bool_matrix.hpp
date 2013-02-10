/**
 * @file   bool_matrix.hpp
 * @author Benjamin Negrevergne <benjamin@neb.dyn.cs.kuleuven.be>
 * @date   Sat Feb  9 15:31:10 2013
 * 
 * @brief  Convinience boolean matrix class for gradual itemset mining.
 * 
 * 
 */

// bool_matrix.hpp
// Made by Benjamin Negrevergne
// Started on  

#ifndef   	_BOOL_MATRIX_HPP_
#define   	_BOOL_MATRIX_HPP_

#include <vector>

/* NOTE: TODO std::vector<bool> are now standard containers with one byte
   elements, this should be replaced by a bitwise implementation for more efficiency*/

/* Square Bool Matrix */


/** 
 * @class BoolMatrix
 *
 * @brief Convinience boolean matrix class for gradual itemset mining.
 * 
 * Boolean Square Matrix. 
 * 
 */
class BoolMatrix : public std::vector<bool> {

private: 
  int N; 

public:
  /** 
   * Create a new boolean matrix of size NxN
   * @param n num of rows or num of cols. 
   * @param fill the matrix with this value. 
   */
  inline BoolMatrix(int N = 0, bool fill = false):
    std::vector<bool>(N*N, fill), N(N){
  }

  /** 
   * \brief Copy constructor.
   */
  inline BoolMatrix(const BoolMatrix &other):
    std::vector<bool>(other), N(other.N){
  }

  /** 
   * \brief Assignment operator.
   */
  inline BoolMatrix &operator=(const BoolMatrix &other){
    std::copy(other.begin(), other.end(), this->begin()); 
    N=other.N; 
    return *this; 
  }

  /** 
   * \brief Assignment operator.
   */
  inline BoolMatrix &bitwise_and(const BoolMatrix &other){
    assert(get_size() == other.get_size()); 
    for(int i = 0; i < N; i++){
      //TODO improve efficiency!
      bool b = (*this)[i];
	b &= other[i];
      (*this)[i] = b; 
    }
    return *this;
  }
  
  /** 
   * \brief Return the size of the matrix. (sqrt of the #of element.)
   */
  inline int get_size() const {return N;}

  /** 
   * \brief Set matrix value [\col, \row] to \v. 
   */
  inline void set_value(int col, int row, bool v){
    assert(col < N && row < N); 
    (*this)[col*N+row] = v; 
  }

  /** 
   * \brief Return matrix value at [\col, \row]. 
   */
  inline bool get_value(int col, int row) const {
    assert(col < N && row < N); 
    return (*this)[col*N+row]; 
  }
  
  /** 
   * \brief Return true if no bit is set in Row \row. 
   */
  inline bool null_row_p(int row) const {
    assert(row < N); 
    int range_start = N * row;
    int range_end = N * (row+1);
    for(int i = range_start; i < range_end; i++)
      if((*this)[i]) return false; 
    return true; 
  }

  

}; 
  

#endif	    /* _BOOL_MATRIX_HPP_ */
