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
 
#ifndef TATREENODE_HXX
#define TATREENODE_HXX


#include "ItTatree.hxx"
#include "RecodeToInt.hxx"

#include <deque>
#include <vector>
#include <map>
#include <stack>
#include <iterator>
    
//! Template class representing a node of a trie data structure.
/**   
   This class represents a node of a trie data structure storing sets of values. 
   The element in the node are stored in lexicographic order.

   The template parameter is the type of the element stored in the trie.   
*/
template<class SetsType >
class TatreeNode
{

    typedef  map< SetsType, TatreeNode<SetsType> * > mapTTatree ;
    typedef typename   map< SetsType, TatreeNode<SetsType> * >::iterator ItMapTTatree ;
    
            
    
    protected:
    
        //! functor used to add an element in a node
        /**
           This functor is used in th push_back() function.
           \see push_back()
        */      
        class insertNode:  public unary_function<SetsType, void>
        {      
                      
              TatreeNode< SetsType > * currentNode ;
              
              int nb; //! number of sets to insert
           
              public:
                                          
                     insertNode( TatreeNode<SetsType> * inNode, int inNb=0 ) { currentNode =  inNode;  nb = inNb ;   }
                     
                     void operator() ( SetsType elem )
                     {                                                                                      
                        TatreeNode<SetsType> * next ;                

                        if( (next=currentNode->items[elem]) == 0 ) // access to the elem and lokk if there is a lin node
                                               // if the elem is not in the node, create it, create a child node, and update count
                        {
                            currentNode  = currentNode->items[elem] = new TatreeNode(nb) ;                                      
                        }   
                        else // the elem is in the node
                        {
                             next->cnt = next->cnt + nb; 
                             currentNode = next ;
                        }     
                    }    
        };
        

    protected:
 
        //! Numbers of sets stored in the previous nodes.
        int     cnt;  
        
        //! Size of the largest set.
        int     max;   
                
        //! Items in the node and corresponding childs nodes
        mapTTatree     items;  
            
    public:
        
        
        //! Constructor
        TatreeNode( int incnt=0 ){     cnt=incnt; max=0;  }
                
        //! Destructor        
        ~TatreeNode()
        {
                    
            if( items.size() )
            {
                 for( ItMapTTatree it = items.begin(); it != items.end(); it ++ )
                      if( it->second ) delete it->second  ;   
            }

        }    
        
        //! Push back a set of element from this node.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of element of type SetsType.
            \param setElement the set of element to insert.
            \param nb the number of times the set is inserted.
        */             
        template< class Container >
        void push_back(  Container & setElement, int nb = 0 )
        {
            insertNode ins( this, nb );
            ins = for_each( setElement.begin(), setElement.end(), ins  );    
        }
             
        //! Push back a set of element from this node.
        /**
            The template parameter represents the input iterators on the elements to insert.
            \param first iterator on the first element
            \param last iterator on the element after the last
            \param nb the number of times the set is inserted
        */             
        template< class InputIterator>
        void push_back( InputIterator & first, InputIterator & last, int nb = 0  )
        {     
             insertNode ins( this, nb);
             ins = for_each( first, last, ins  ); 
        } 
         
        friend class ItTatree_base<SetsType>; 
        friend class Tatree_base<SetsType> ;      
        friend class ItTatree<SetsType>; 
        friend class Tatree<SetsType> ;
        
        
        //! Return the number of occurence of the set stored in the previous nodes.
        int number(){ return cnt; }  
        
        //! Return the nodes and their child nodes
        mapTTatree & childs() { return items;}     
  
} ; 


#endif
