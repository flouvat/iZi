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
 
#ifndef TATREE_BASE_HXX
#define TATREE_BASE_HXX

#include "ItTatree_base.hxx"
#include "RecodeToInt.hxx"
#include "TatreeNode.hxx"

#include <deque>
#include <vector>
#include <map>
#include <stack>
#include <iterator>

   
    
//! Template class representing a trie data structure.
/**   
   The operations on a Tatree_base are limited. For example, we cannot directly acces to the transactions by using ++ and *.
   The advantage of that type of structure is that it is lighter than one with more operations.
   Consequently code dedicated to such class will be more efficient.
   
   The template parameter is the type of the element stored in the trie.   
*/
template<class SetsType >
class Tatree_base
{
    protected:
         
        //! Root node of the trie.
        TatreeNode<SetsType> * root ;
        
        //! Functor used to recode and order elements
        RecodeToInt<SetsType> * recode ;
        
        //! number of elements in the trie
        int nbSets;
                    
    public:
        
        //! Used to have the same syntax tha the STL 
        typedef ItTatree_base<SetsType> iterator; 
        
        //! Constructor
        Tatree_base( ){  root = 0;  nbSets = 0; recode = 0;}
                
        //! Destructor        
        ~Tatree_base()
        {
                 
            if( root )
                delete root;
               
            if( recode )
                delete recode;                               

        }    
        
        //! Push back a set of element in the trie.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of element of type SetsType.
            \param setElement the set of element to insert in the trie
            \param nb the number of times the set is inserted
        */             
        template< class Container >
        void push_back(  Container & setElement, int nb = 1 )
        {
            if( ! root )
                root = new TatreeNode<SetsType>() ; 
                        
            if( recode )
            {
                root->push_back( (*recode)( setElement), nb ) ;
            }
            else
            {
                root->push_back( setElement, nb ) ;
            }
            nbSets = nbSets + nb ;       
        
        }
             
        //! Push back a set of element in the trie.
        /**
            The template parameter represents the input iterators on the elements to insert in the trie.
            \param first iterator on the first element
            \param last iterator on the element after the last
            \param nb the number of times the set is inserted
        */             
        template< class InputIterator>
        void push_back( InputIterator & first, InputIterator & last, int nb = 1  )
        {     
            if( ! root )
                root = new TatreeNode<SetsType>() ;             

            if( recode )
                root->push_back( (*recode)( first, last), nb ) ;
            else
                root->push_back( first, last, nb  ) ;
            nbSets = nbSets + nb ;       

        } 
        
        void setRecode( RecodeToInt<SetsType> * inrecode ) { recode =inrecode; }
      
        //! Function that returns the number of sets in the trie 
        int size(){ return nbSets; }
      
        //! Return an iterator on the root node. 
        ItTatree_base<SetsType> begin() { return ItTatree_base<SetsType>( this );  }
        
        //! Return an iterator on the root node. 
        ItTatree_base<SetsType> beginRoot() { return ItTatree_base<SetsType>( this );  }
            
        //! Return an iterator on the element after the last element stored. 
        ItTatree_base<SetsType> end() { return ItTatree_base<SetsType>() ; }         
                                         
        friend class ItTatree_base<SetsType>;              
  
} ; 


#endif
