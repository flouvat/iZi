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
 
#ifndef ITTATREE_HXX
#define ITTATREE_HXX

#include <vector>
#include <map>
#include <deque>
#include <iterator>

template<class SetsType > class  Tatree;
template<class SetsType > class  TatreeNode;

//! Iterator on a Tatree data structure (trie).
/**
    The template parameter SetsType is the type of the element stored in the trie.
    \see Tatree
*/
template<class SetsType >
class ItTatree 
{ 
        
        typedef  map< SetsType, TatreeNode<SetsType> * > mapTTatree ;
        typedef typename   map< SetsType, TatreeNode<SetsType> * >::iterator ItMapTTatree ;
        typedef pair< TatreeNode< SetsType > *,  ItMapTTatree > pairTatIt ; // the first is the parent node and the second an iterator on the current node
        typedef typename deque< pairTatIt >::iterator ItDeqPair;
                    
         //! Container with the set correponding to the current leaf node.
        vector<SetsType>  currentSet;  
                    
        //! Extract the next set
        bool nextSet( ItTatree * itTa ) ;   
                   
    protected:
          
        //! Stack of nodes and iterators on nodes of the current branch (the top element corresponds to the current node)
        deque< pairTatIt >  prevNode;
        
        //! Current trie.
        Tatree<SetsType> * trie;                     
                                                       
     public:
          
        //! Contructor.
        ItTatree( Tatree<SetsType> * intrie=0 ); 
                                
        //! Copy contructor.         
        ItTatree( const ItTatree & it ){ trie = it.trie; prevNode = it.prevNode; } 
        
        //! Affectation operator.
        ItTatree & operator = ( const ItTatree & it ){ trie = it.trie; prevNode = it.prevNode; } 
        
        //! Destructor.
        ~ItTatree(){};
        
        //! Increment operator.
        /**
         get the next set stored.
        */
        ItTatree &  operator++(){ nextSet(this); return (*this); }  
        ItTatree  operator++(int){ nextSet(this); return ItTatree(*this); };       
        
        //! Return the current set in the trie
        vector<SetsType> & operator * () 
        { 
            currentSet.resize( 0 ); // set the size to 0 but keep the memory
            currentSet.reserve( prevNode.size() ); // reserve memory
            
            for( ItDeqPair it = prevNode.begin() ; it != prevNode.end() ; ++it )
            {                
                 if( trie->recode )                  
                     currentSet.push_back( trie->recode->remap(it->second->first) );
                 else
                     currentSet.push_back( it->second->first );
            }
            
            return currentSet; 
        }

        //! Return the current set in the trie                    
        vector<SetsType> * operator -> () 
        { 
            currentSet.resize( 0 ); // set the size to 0 but keep the memory
            currentSet.reserve( prevNode.size() ); // reserve memory
            
            for( ItDeqPair it = prevNode.begin() ; it != prevNode.end() ; ++it )
            {              
                 if( trie->recode )
                     currentSet.push_back( trie->recode->remap(it->second->first) );
                 else
                     currentSet.push_back( it->second->first );
            }
            return &currentSet; 
        }                  
        
        //! Equality operator
        bool operator == ( const ItTatree & it ) const 
        {
           if( prevNode.size() )
           {
               if( it.prevNode.size() )  
                   return ( prevNode.back().first == it.prevNode.back().first && prevNode.back().second == it.prevNode.back().second ); 
               else
                   return false;
           }
            else if(  it.prevNode.size() )
               return false ;
           else
               return trie == it.trie;
        }
        
        //! Difference operator 
        bool operator != ( const ItTatree & it ) const 
        {
           if( prevNode.size() )
           {
               if( it.prevNode.size() )  
                   return (prevNode.back().first != it.prevNode.back().first || prevNode.back().second != it.prevNode.back().second); 
               else 
                    return true;
           }
           else if(  it.prevNode.size() )
               return true ;
           else
               return trie != it.trie;
        }
        
        //! Function returning the element in the current node
        SetsType element()
        {                        
            if( trie->recode ) 
                return trie->recode->remap( prevNode.back().second->first); 
            else 
                return prevNode.back().second->first; 
        }
        
         //! Function returning the internal id of the element in the current node
        int elementId() { return prevNode.back().second->first;}

        //! return the measure store in the node  associated to the set
        int measure() { return prevNode.back().second->second->cnt ; }
          
        //! Function returning if the iterrator is on a leaf node
        /**
           \return true if the current node is a leaf node.
        */        
        bool isaLeaf() { if( prevNode.size() ) return prevNode.back().second.second->items.size() == 0 ; else return true;}
                
        //! Move the iterator on the previous (wrt order of elements) child node of the current parent node
        /**
           If the current node is the root node return the end iterator.
           \return an iterator on the next (wrt order of elements) child node if exists, else return default iterator.
        */
        ItTatree & rnextChild()
        {
            if( prevNode.size() )
            {
                if( prevNode.back().second != prevNode.back().first->items.begin() ) // this is not the first chil node               
                    --(prevNode.back().second );
                else
                    (*this) = ItTatree() ;  // end iterator
                                                    
            }                            
            return (*this);               
        }        
                 
        //! Move the iterator on the next (wrt order of elements) child node of the current parent node
        /**
           If the current node is the root node return the end iterator.
           \return an iterator on the next (wrt order of elements) child node if exists, else return default iterator.
        */
        ItTatree & nextChild()
        {
            if( prevNode.size() )
            {
                if( prevNode.back().second != prevNode.back().first->items.end() ) // this is not the last chil node               
                {
                    ++(prevNode.back().second );
                    
                    if( prevNode.back().second == prevNode.back().first->items.end() )
                        (*this) = ItTatree() ;  // end iterator                                                    
                }
                else
                    (*this) = ItTatree() ;  // end iterator                                                    
            } 
            
            return (*this);               
        }
        
        //! Move the iterator on the current node on the first child node.
        /**
            \return the iterator on the child node.
        */
        ItTatree & beginChildNode()
        {
            if( prevNode.size() )
            {
                 TatreeNode< SetsType> *child =  prevNode.back().second->second ;

                if( child->items.size() ) // this is not a leaf
                {
                        prevNode.push_back( make_pair( child,  child->items.begin() ) ) ;                     
                }
                else
                    (*this) = ItTatree() ;                    // end iterator
                                                    
            } 
            else // root node
            if( trie )
            {
                    prevNode.push_back( make_pair( trie->root,  trie->root->items.begin() ) )  ;                                 
            }
  

            return (*this);                                                       
        }
        
        //! Move the iterator on the current node on the first in reverse order child node.
        /**
            \return the iterator on the child node.
        */
        ItTatree & rbeginChildNode()
        {
            if( prevNode.size() )
            {
                 TatreeNode< SetsType> *child =  prevNode.back().second->second ;

                if( child->items.size() ) // this is not a leaf
                {
                    if( child->items.size()> 1 )
                        prevNode.push_back( make_pair( child,  (++child->items.rbegin()).base() ) ) ; 
                    else
                        prevNode.push_back( make_pair( child,  child->items.begin() ) ) ;                     
                }
                else
                    (*this) = ItTatree() ;                    // end iterator
                                                    
            } 
            else // root node
            if( trie )
            {
                if( trie->root->items.size() > 1 )
                    prevNode.push_back( make_pair( trie->root,  (++trie->root->items.rbegin()).base() ))  ;                
                else
                    prevNode.push_back( make_pair( trie->root,  trie->root->items.begin() ) )  ;                                 
            }

                 
            return (*this);                                                       
        }
        
 
        friend class Tatree<SetsType>;

};

//! Contructor.
/**
    Set the iterator on the first set, ie first leaf node.
*/
template<class SetsType  >
ItTatree<SetsType >::ItTatree( Tatree<SetsType> * intrie )
{ 
     trie = intrie ;                            
     
     if( trie )
     {
         TatreeNode<SetsType> * currNode = intrie->root; 
               
         if( currNode )
         {
            // search the first set
            while( currNode->items.size() != 0 )
            {
                prevNode.push_back( make_pair( currNode, currNode->items.begin() ) ) ;  // update the stack  
                currNode = currNode->items.begin()->second;      
            }         
         } 
     }    
}; 
          
//! Etract the next set
template<class SetsType >
bool ItTatree<SetsType>::nextSet( ItTatree * itTa ) 
{
     TatreeNode<SetsType> * currNode = 0 ;
     
     if( itTa->prevNode.size() == 0 ) // this the root node
     {
         itTa->prevNode.push_back( make_pair( itTa->trie->root,  itTa->trie->root->items.begin() ) ) ;                  
         return 1;
     }     
     else
     {             
         while( itTa->prevNode.size() != 0 )
         {
             pairTatIt precedent = itTa->prevNode.back() ; // go to the parent node
             itTa->prevNode.pop_back() ;                         
    
             ++(precedent.second)  ; // explore the next element in the parent node
             
             currNode = precedent.first ;
             
             ItMapTTatree * itCurr = &  precedent.second ;                     
             
             if( *itCurr != currNode->items.end() ) // there is another element in the parent node
             {
                itTa->prevNode.push_back( make_pair( currNode, *itCurr) ) ;
           
                currNode =  (*itCurr)->second ; // next node in the trie
       
                while( currNode->items.size() != 0 )
                {
                    itTa->prevNode.push_back( make_pair( currNode, currNode->items.begin() ) ) ;  // update the stack  
                    currNode = currNode->items.begin()->second;  
                }                
                return 1;                                              
             }                 
         }    
         
    }
     
     itTa->trie = 0 ; // end iterator        
         
     return 1 ;
}

#endif
