/*
 *  Copyright (C) 2006 Frédéric Flouvat
 *  Written by Frédéric Flouvat
 *  Updated by Frédéric Flouvat
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef TATREE_HXX
#define TATREE_HXX


#include "Tatree_base.hxx"
#include "ItTatree.hxx"
#include "RecodeToInt.hxx"
#include "TatreeNode.hxx"

#include <deque>
#include <vector>
#include <map>
#include <stack>
#include <iterator>

   
    
//! Template class representing a trie data structure (this is an "interface" of the Tatre_base class).
/**   
   This class differs from the mother class Tatree_base by providing additional methods and the access to a more complete iterator.
   Using this class, it is possible to explore all the sets stored in trie using the iterators ++.
   The template parameter is the type of the element stored in the trie.   
*/
template<class SetsType >
class Tatree: public Tatree_base<SetsType>
{                    
    public:
        
        //! Used to have the same syntax tha the STL 
        typedef ItTatree<SetsType> iterator; 
        
        //! Constructor
        Tatree( ):Tatree_base<SetsType>() {}
                
        //! Destructor        
        ~Tatree(){}
        
        //! Return an iterator on the first element stored. 
        ItTatree<SetsType> begin() { return ItTatree<SetsType>( this );  }
        
        //! Return an iterator on the root node. 
        ItTatree<SetsType> beginRoot() { ItTatree<SetsType> it; it.trie = this;  return it;  }
            
        //! Return an iterator on the element after the last element stored. 
        ItTatree<SetsType> end() { return ItTatree<SetsType>() ; }         
        
                           
        //! Function that print to screen the sets stored in the trie
        void print() {  for_each( begin(),end(), printContainerOfContainer( this->recode) );  }     
        
        //! Return the root node
        TatreeNode<SetsType> * getRoot(){ return this->root ;}
        
        //! Set the recode functor to the functor passed in parameter
        void setRecode( RecodeToInt<SetsType> * inrec ){ this->recode = inrec; }
        
        //! Get the recode functor.
        RecodeToInt<SetsType> * getRecode(){ return this->recode ; }
                               
        friend class ItTatree<SetsType>;              
  
} ; 


#endif
