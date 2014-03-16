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

#ifndef TESTSUBSETS_HXX
#define TESTSUBSETS_HXX

#include "PTree.hxx"

//! Functor testing all the k-1 subsets of the itemset passed in parameter wrt to a binary predicate
template< class SetsType> 
class TestSubsets
{

      vector<SetsType> buffer;   
      
  public:
                   
      template< class IteratorCand, class Functor   >
      bool operator()( IteratorCand  &  itemsetIt, Functor  funct  )
      {
            buffer.reserve( itemsetIt.length() -1 );  
            buffer.resize( 0 ); // set the buffer to 0 but does not de allocate memory 
             
            IteratorCand itParent ; // iterator on the parent node
            IteratorCand itSearch ; // iterator used to search elem in child nodes
            SetsType idParent ; // internal id
            
            buffer.push_back( itemsetIt.element() );// save the item to search for subsets, ie first subset tested
            
            itParent = itemsetIt.parentNode(); // go to parent node to test other subsets      
            idParent = itParent.element(); 
            
            if( funct( itemsetIt,  itParent) )
                return true;
            
            itParent = itParent.parentNode();        
            
            while( itParent !=  IteratorCand() ) // search all the subsets and test
            {                  
                int i = buffer.size() -1 ;
            
                itSearch = itParent.childNode( buffer[i] );
            
                while( i> 0 && itSearch != IteratorCand()  ) // go down the child nodes to find a subset
                {              
                    --i;
                    itSearch = itSearch.childNode(buffer[i]);
                }    
            
                if( i== 0  && itSearch != IteratorCand()  ) // a subset is in the trie
                {  
                    if( funct( itemsetIt,  itSearch)   )
                        return true;             
                }    
            
                buffer.push_back( idParent ) ; 
                
                idParent = itParent.element() ;
            
                itParent = itParent.parentNode() ;                                   
            }              
            return false;            
      }
};

#endif
