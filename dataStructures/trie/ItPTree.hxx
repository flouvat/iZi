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
 
#ifndef ITPTREE_HXX
#define ITPTREE_HXX

#include <vector>
#include <iterator>

#include "Boolean.hpp"
#include "RecodeToInt.hxx"


template<class SetsType, class Measure> class  PTree;
template<class SetsType, class Measure> class  Node;

//! Iterator on a PTree (trie data structure).
/**
    The template parameter SetsType is the type of the elements stored in the trie.
    The template parameter Measure is the type used in the "boolean" vector to code the presence of an element.
    \see PTree.
*/
template<class SetsType,class Measure=Boolean>
class ItPTree 
{ 
public:      
        //! search the last node of the next itemset.
        void nextItemset( ItPTree * );
        
        //! Get the set (internal id ) stored in the node pointed by the iterator passed in parameter. 
        /**
           \param inset vector of SIZE it->length where the elements are stored.
           \param it iterator on the last node storing the last element of the set to extract.
        */
        void getSet( vector<int> * inset, ItPTree * it);
        
        //! used for sets extraction by * and ->
        vector<int> * internalset;
        vector<SetsType> * set;
        
        
    protected:
      
        //! Current Node   
        Node<SetsType,Measure> * ptr;
        
        //! Internal id (an integer) of the curent element
        int internalId ; // -1 for the root
        
        //! Recode the elements in integers
        RecodeToInt< SetsType > * recode ;
        
        //! Number of nodes from the root node
        int height ;
                
    public:
      
        //! Default constructor.
        ItPTree( Node<SetsType,Measure> * currnode = 0, int ininternalId = 0 , RecodeToInt< SetsType > * inrecode = 0 , int inheight = 0 ){ ptr = currnode; internalId = ininternalId; recode = inrecode ; height = inheight ; internalset=0;set=0; }       
        
        //! Contructor setting the iterator on the first set of the trie.        
        ItPTree( PTree<SetsType,Measure> *  );  
        
        //! Contructor setting the iterator on the first leaf of the trie.        
        ItPTree( Node<SetsType,Measure> *, RecodeToInt<SetsType> *  );  

        //! Copy constructor
        ItPTree( const ItPTree & it ){ internalset=0;set=0;  ptr = it.ptr; internalId = it.internalId; recode = it.recode; height = it.height;  }
        
        //! Affectation operator
        ItPTree & operator = ( const ItPTree & it ){ if( internalset ) delete internalset; if( set ) delete set ; internalset=0;set=0;  ptr = it.ptr; internalId = it.internalId;  recode = it.recode; height = it.height; return (*this); }
        
        //! Destructor
        ~ItPTree(){ if( internalset ) delete internalset; if( set ) delete set ;}
        
        //! Operator returning the current set.
        vector<SetsType> & operator * () 
        { 
                      
            if( ! internalset )   
                internalset = new vector< int >;          
                
            if( ! set )   
                set = new vector< SetsType >; 
            
            // search the current set     
            getSet( internalset, this);     

            // transform the internal id of the set into their original value
            if( recode )            
                recode->remap( internalset, set ) ;
         
            return *set;                    
        }
             
        //! Operator returning a pointer on the current set.
        vector<SetsType> * operator -> () 
        { 
            if( ! internalset )   
                internalset = new vector< int >;          
                
            if( ! set )   
                set = new vector< SetsType >; 
            
            // search the current set                    
            getSet( internalset, this);   

            // transform the internal id of the set into their original value
            if( recode )            
                recode->remap( internalset, set ) ;

            return set;                     
        }
        
        //! Equality operator.
        bool operator == ( const ItPTree & it ) const {  return ( ptr == it.ptr && internalId == it.internalId ); }        
        
        //! Difference operator.
        bool operator != ( const ItPTree & it ) const { return ( ptr != it.ptr || internalId != it.internalId ) ; }
        
        //! Move the iterator on the next set stored (not necessarely on a leaf node for subsets).
        ItPTree &  operator++(){  nextItemset( this );  return (*this); };  
        
        //! Move the iterator on the next set stored (not necessarely on a leaf node for subsets).
        ItPTree  operator++(int){ nextItemset( this); return ItPTree(*this); };        
                               
        //! Function returning if the iterrator is on a leaf node
        /**
           \return true if the current node is a leaf node.
        */
        bool isaLeaf(){  int index = internalId - ptr->offset ; if( index >=0 && ptr->childs && index < ptr->childs->size() &&  (*ptr->childs)[index] ) return false ; else return true ; }
        
        //! Return an iterator on the parent node 
        /**
           \return an iterator on the parent node if exists, else return default iterator.
        */
        ItPTree parentNode()
        { 
            ItPTree parent;
            
            if( ptr && internalId!= -1 )
            { 
                if( !ptr->parent  ) // root node
                {
                    parent.internalId = -1 ; // mark as the root node
                    parent.ptr = ptr; 
                    parent.height = 0;                   
                }    
                else
                {
                    parent.internalId = ptr->id   ; 
                    parent.ptr = ptr->parent ; 
                    parent.height = height - 1;
                }    
                parent.recode = recode ;
            } 
            
            return parent;
        }
        
        //! Return an iterator on a child node (of the current node) containing the element elem .
        /**
            return the default iterator if there is no child node with the element.    
        */
        ItPTree childNode( SetsType elem )
        {
             
            int intid = (*recode)( elem );    
            ItPTree result ;
            int childindex;
            int index = internalId - ptr->offset;
            
            if( internalId == -1 ) // virtually the root node 
            {
                
                childindex = intid - ptr->offset ;

                if( childindex >= 0 
                    && childindex < ptr->cnts->size()
                    && (*ptr->cnts)[childindex] != Measure() )
                {
                    result.ptr = ptr;
                    result.internalId = intid ;
                    result.recode = recode ;
                    result.height = 1 ;
                }    
                
            }
            else
            {    
                if( ptr->childs &&  index < ptr->childs->size() 
                    && (*ptr->childs)[index] ) // there is a child node
                {
                    childindex = intid - ( (*ptr->childs)[index] )->offset ;

                    if( childindex >= 0 
                        && childindex < ((*ptr->childs)[index])->cnts->size()
                        && (*((*ptr->childs)[index])->cnts)[childindex] != Measure() ) //the elemnt is in the chil node
                        {
                            result.ptr = (*ptr->childs)[index] ;
                            result.internalId = intid ;
                            result.recode = recode ; 
                            result.height = height + 1;                       
                        }    
                }            
            }    

            return result;
        }    

        //! Return an iterator on the next (wrt order of elements) child node of the current parent node
        /**
           \return an iterator on the next (wrt order of elements) child node if exists, else return default iterator.
        */
        ItPTree nextChildNode()
        { 
            ItPTree child;
            
            int index = internalId - ptr->offset +1 ;          

            child.ptr = ptr ;
            child.recode = recode ;
            child.height = height; 

            while( index < ptr->cnts->size() && (*ptr->cnts)[index] == Measure() )             
                index++ ;  

            if( index == ptr->cnts->size() ) // no other child node
            {
                child.internalId =0 ;
                child.ptr = 0 ;
                child.recode = 0;            
                child.height = 0;  
            }   
            else
                child.internalId = index + ptr->offset ;
            
            return child ;
        }
                               
        //! Return an iterator on the next leaf (exploring the tree from the left to the right)
        ItPTree nextLeaf();

        //! Return an iterator on the next leaf (exploring the tree from the  right to the left)
        ItPTree rnextLeaf();
        
        //! Return the depth from the root node.
        int length() { return height;}
        
        //! return the measure store in the node adn associated to the set
        Measure & measure() { return (*ptr->cnts)[ internalId - ptr->offset  ] ; } 


         //! return the measure store in the node adn associated to the set
        void  measure( Measure  inmeasure ) { (*ptr->cnts)[ internalId - ptr->offset  ] = inmeasure  ; }
        
         //! Function returning the element in the current node
        SetsType element() {  if( internalId != -1 ) return recode->remap( internalId ) ; else return SetsType();}             
        
         //! Function returning the internal id of the element in the current node
        int elementId() { return internalId;}
        
        //! return a pointer on the current node
        Node<SetsType,Measure> * node() { return ptr ; }
        
        //! return the current set (with internal ids) from the curent node  
        vector<int> * getInternalset()
        {
            if( ! internalset )   
                internalset = new vector< int >;          
                
            getSet( internalset, this);                        
             
            return internalset ;        
        }  
        
        friend class Node< SetsType, Measure >;
        friend class PTree<SetsType,Measure>;
};


//! search the last node of the next itemset.
template<class SetsType,class Measure>
void ItPTree< SetsType, Measure >::nextItemset( ItPTree * it )
{
    Node< SetsType, Measure > * current =  it->ptr ;
    
    if( current )
    {    
        int i = it->internalId  - current->offset ;

        // the precedent set extracted  was a subsets 
        // as a consequence   the child node have to be explored
        if( current->childs  && i< current->childs->size() && (*current->childs)[i] ) // if the precedent set study was a subset
        {  
             current = (*current->childs)[i] ;
             i = 0 ;
             ++(it->height) ;
                   
        }
        else
            i++;
                 
        // search the next itemset coded in the current node 
        while( current && current->cnts && i < current->cnts->size() && ! current->existVal( i ) )
            i++ ;
            
        // no other itemsets from the same node, go to the parent node and search itemset            
        while( current && current->cnts && i >= current->cnts->size() ) 
        {

             if(  current->parent )
             {
                
                  i = current->id + 1 - current->parent->offset ; 
                   --(it->height) ;
             }

             current = current->parent ;         
    
             // search the next itemset coded in the current node 
             while( current && current->cnts &&i < current->cnts->size() && ! current->existVal( i ) )
             {
                    i++ ;      
             }             
        } 
      
        // the next itemset have an item in the child nodes            
        while( current && current->childs && i< current->childs->size() && (*current->childs)[i] ) 
        {

            // test if the current set is a subset
            if( (current->getCnt( i )   ) != Measure(true) ) // if this is a subset        
            { 
                it->ptr = current ;  
                it->internalId = i + current->offset;
                break;
            }
            
            ++(it->height) ;   
            it->ptr = current ;  
            it->internalId = i + current->offset;
                
            current = (*current->childs)[i] ;
            i=0;                    
        }
   
        if( ! current )
            it->internalId = 0 ;   
        else
            it->internalId = i + current->offset;
           
         it->ptr = current ;  
    }
}


//! Get the set (internal id ) stored in the node pointed by the iterator passed in parameter. 
/**
   \return a pointer on the set.
*/
template<class SetsType,class Measure>
void ItPTree< SetsType, Measure >::getSet( vector<int> * inset , ItPTree * it)
{
     
    Node<SetsType,Measure> * currnode = it->ptr ;
    int size = it->height ;  
    
    if( currnode )
    {        
        inset->resize( size );
        
        --size;  
    
  
        (*inset)[size] = it->internalId ; // id of the element of the cuurent node
        --size;       
  
        if( size >= 0 )
        {
    
    
            (*inset)[size] = currnode->id ; // internal id of the parent node
    
   
            currnode = currnode->parent ;
     
             while( currnode && currnode->id != -1 && size>0 ) // go to the root node and read the elements stored
             {   
                    --size;          
   
                   (*inset)[size] = currnode->id;
                    currnode = currnode->parent ;         
                    
             }  
             
        }
        
    }
}

//! Contructor setting the iterator on the first set of the trie.        
template<class SetsType,class Measure>
ItPTree< SetsType, Measure >::ItPTree(PTree< SetsType, Measure > * t )
{

    Node< SetsType, Measure > *currNode = t->head;
    
    ptr = 0 ;
    internalId=0;
    recode = 0 ;
    internalset=0;set=0; 
    height = 0 ; 
                                                                  
    if( currNode && currNode->cnts  )
    {
        recode = &t->recode ;
        
        internalset=0;
        set=0; 
        height = 0 ;
        
        int index = 0 ;
    

        // we search the first itemset in the trie
        
        while( currNode && currNode->cnts && currNode->existVal( 0 ) )
        {
               ptr = currNode;
               
               ++height ;
               
      			// we continue searching with the next items

           		if( currNode->childs && currNode->childs->size() && (*currNode->childs)[0] )
                {	
            
                    // test if the current set is a subset
                    if( (currNode->getCnt(0 ) ) != Measure(true) ) // if this is a subset
                    {
                        ptr = currNode ;  
                        internalId =  ptr->offset ;
                        break;
                    }
                             
           		    currNode = (*currNode->childs)[0] ;
    	        }    
           		else // we have check all the items of the current itemset strored in vect                     
                    currNode = 0 ;                      				
    	}                                               
    	
    	internalId =  ptr->offset ;
     }  
     
}

//! Contructor setting the iterator on the first leaf of the trie.        
template<class SetsType,class Measure>
ItPTree< SetsType, Measure >::ItPTree( Node<SetsType,Measure> * rootNode, RecodeToInt<SetsType> * inrec )
{                     
    Node< SetsType, Measure > *currNode = rootNode;
    recode = inrec ;
    ptr = currNode;
    int index = 0 ;
    
    internalset=0;set=0; 
    height = 0 ; 
    internalId=0;
                                                                 
    if( currNode && currNode->cnts  )
    {
        // we search the first itemset in the trie        
        while( currNode && currNode->cnts  && currNode->existVal( 0 ) )
        {
               ptr = currNode;
               ++height;
               
    			// we continue searching with the next items
       
           		if( currNode->childs && currNode->childs->size() )
                {	                                                     
           		    currNode = (*currNode->childs)[0] ;
    	        }    
           		else // we have check all the items of the current itemset strored in vect                     
                    currNode = 0 ;                      				
    	}                                               
    	internalId =  ptr->offset ;
     }     
                                                         
}

//! Return an iterator on the next leaf from the curent node
template<class SetsType,class Measure>
ItPTree< SetsType, Measure > ItPTree< SetsType, Measure >::nextLeaf()
{    
    ItPTree it = *this ;
        
    Node< SetsType, Measure > * current =  it.ptr ;
    
    if( current )
    {
        int i = it.internalId - current->offset ;
        
        i++;
            
        // search the next itemset coded in the current node 
        while( current && i < current->cnts->size() && ! current->existVal( i ) )
            i++ ;
    
        // no other itemsets from the same node, go to the parent node and search itemset            
        while( current && i >= current->cnts->size() ) 
        {
             if(  current->parent )
             {
                  i = current->id + 1 - current->parent->offset ; 
                  --(it.height) ;
             }
    
             current = current->parent ;         
    
             // search the next itemset coded in the current node 
             while( current && i < current->cnts->size() && ! current->existVal( i ) )
                    i++ ;                 
        }  
        
        // the next itemset have item in the chid nodes            
        while( current && current->childs && i< current->childs->size() && (*current->childs)[i] ) 
        {
            ++(it.height) ;
            current = (*current->childs)[i] ;
            i=0;  
                  
        }
    
        if( current )
        {               
            it.internalId = i + current->offset;
        }
        else    
            it.internalId = 0 ;
        
        it.ptr = current ;  
    }
        
    return it;  
}

//! Return an iterator on the next leaf from the curent node (exploring the tree from the  right to the left)
template<class SetsType,class Measure>
ItPTree< SetsType, Measure > ItPTree< SetsType, Measure >::rnextLeaf()
{    
    ItPTree it = *this ;
        
    Node< SetsType, Measure > * current =  it.ptr ;

    if( current )
    {
        
        int i = it.internalId - current->offset ;
        
        --i;
    
        // search the next itemset coded in the current node 
        while( current && i >= 0 && ! current->existVal( i ) )
            --i ;
    
        // no other itemsets from the same node, go to the parent node and search itemset            
        while( current && i <0) 
        {
             if(  current->parent )
             {
                  i = current->id - 1 - current->parent->offset ; 
                  --(it.height) ;
             }
    
             current = current->parent ;         
    
             // search the next itemset coded in the current node 
             while( current && i >= 0 && ! current->existVal( i ) )
                    --i ;                 
        }  
    
        // the next itemset have item in the chid nodes            
        while( current && current->childs && i>=0  && i<current->childs->size() && (*current->childs)[i] ) 
        {
            ++(it.height) ;
            current = (*current->childs)[i] ;
            i = current->cnts->size() - 1 ;
            
            // search the last element in the current node 
            while( current && i >= 0 && ! current->existVal( i ) )
                        --i ;  
        
        }
    
        if( current )
        {               
            it.internalId = i + current->offset;
        }
        else    
            it.internalId = 0 ;
        
        it.ptr = current ;  
    }
    
    return it;  
}

#endif
