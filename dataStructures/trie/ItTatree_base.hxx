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
 
#ifndef ITTATREE_BASE_HXX
#define ITTATREE_BASE_HXX

#include <vector>
#include <map>
#include <deque>
#include <iterator>

template<class SetsType > class  Tatree_base;
template<class SetsType > class  TatreeNode;

//! Iterator on a Tatree_base (trie data structure).
/*!
   The operations on a Tatree_base are limited. For example, we cannot directly acces to the transactions by using ++ and *.
   The advantage of that type of structure is that it is lighter than one with more operations.
   Consequently code dedicated to such class will be more efficient.
   
   The template parameter SetsType is the type of the element stored in the trie.
   The template parameter Container is the container use to store the set of elements.   
*/
template<class SetsType >
class ItTatree_base 
{ 
        
        typedef  map< SetsType, TatreeNode<SetsType> * > mapTTatree ;
        typedef typename   map< SetsType, TatreeNode<SetsType> * >::iterator ItMapTTatree ;
        typedef pair< TatreeNode< SetsType > *,  ItMapTTatree > pairTatIt ;
        typedef typename deque< pairTatIt >::iterator ItDeqPair;
                    
                   
    protected:
         
       //! Pointer on the current node.
       TatreeNode< SetsType > * node ;
       
       //! Iterator on current element
       ItMapTTatree  itElem ; 
        
        //! Current trie.
        Tatree_base<SetsType> * trie;                     
                                                       
     public:
          
        //! Contructor.
        ItTatree_base( Tatree_base<SetsType> * intrie=0 ){ trie = intrie ;  node = 0 ; }; 
                                
        //! Copy contructor.         
        ItTatree_base( const ItTatree_base & it ){ trie = it.trie; node =it.node; itElem=it.itElem;} 
        
        //! Affectation operator.
        ItTatree_base & operator = ( const ItTatree_base & it ){ trie = it.trie;  node =it.node; itElem=it.itElem; return (*this) ;} 
        
        //! Destructor.
        ~ItTatree_base(){};
                        
        //! Equality operator.   
        bool operator == ( const ItTatree_base & it ) const 
        {
               return node == it.node && itElem == it.itElem ;               
        }
        
        //! Difference operator.
        bool operator != ( const ItTatree_base & it ) const 
        {
            if( node == 0 &&  it.node == 0)
                return false;
            if( node == it.node )
                return itElem != it.itElem ;
            else if( itElem == it.itElem  )
                 return false ;
            else
                return true ;           
        }
        
        //! Function returning the id of the element in the current node
        SetsType element()
        { 
            if( trie->recode )
                 return trie->recode->remap( itElem->first );
            else  
                 return  itElem->first;                
        }
        
         //! Function returning the internal id of the element in the current node
        int elementId() { return itElem->first;}

        //! return the measure store in the node  associated to the set
        int measure() 
        { 
            return itElem->second->cnt;            
        }
                  
        //! Move the iterator on the previous (wrt order of elements) child node of the current parent node
        /*!
           If the current node is the root node return the end iterator.
           For efficiency reason the returne iterator does not contain the underlying set stored from the node.     
           \return an iterator on the previous (wrt order of elements) child node if exists, else return default iterator.
        */
        ItTatree_base & rnextChild()
        {
            if( node->items.size() && itElem != node->items.begin() ) 
                --itElem;    
            else
                (*this) = ItTatree_base() ;                    // end iterator
                 
            return (*this);               
        }
        
        //! Move the iterator on the next (wrt order of elements) child node of the current parent node
        /*!
           If the current node is the root node return the end iterator.
           For efficiency reason the returne iterator does not contain the underlying set stored from the node.     
           \return an iterator on the next (wrt order of elements) child node if exists, else return default iterator.
        */
        ItTatree_base & nextChild()
        {
            if( node->items.size() && itElem != node->items.end() )
            { 
                ++itElem;    
                if( itElem == node->items.end() )
                    (*this) = ItTatree_base() ; 
            }
            else
                (*this) = ItTatree_base() ;                    // end iterator
                 
            return (*this);               
        }
        
                         
        //! Move the iterator on the current node on the first in reverse order child node.
        /*!
            \return the iterator on the child node.
        */
        ItTatree_base & rbeginChildNode()
        {        
            if( node )
            {
                TatreeNode< SetsType> *child =  itElem->second ;
            
                if( child->items.size() ) // this is not a leaf
                {
                    node = child ;
                    if( child->items.size()> 1 )
                        itElem = (++child->items.rbegin()).base()  ; 
                    else
                        itElem = child->items.begin()  ;                                         
                }
                else
                    (*this) = ItTatree_base() ;                    // end iterator
            } 
            else // root node
            if( trie )
            {              
                node = trie->root ;
                
                if( node->items.size() > 1 )
                    itElem = (++node->items.rbegin()).base() ;
                else
                    itElem = node->items.begin();                                                
            }
            return (*this);                                                       
        }
        
        
        //! Move the iterator on the current node on the first child node.
        /*!
            \return the iterator on the child node.
        */
        ItTatree_base & beginChildNode()
        {        
            if( node )
            {
               
                TatreeNode< SetsType> *child =  itElem->second ;
            
                if( child->items.size() ) // this is not a leaf
                {
                    node = child ;
                    itElem = child->items.begin()  ;                                         
                }
                else
                    (*this) = ItTatree_base() ;                    // end iterator

            } 
            else // root node
            if( trie )
            {              
                node = trie->root ;
                itElem = node->items.begin();                                                
            }
            return (*this);                                                       
        }
        
        friend class Tatree_base<SetsType>;

};


#endif
