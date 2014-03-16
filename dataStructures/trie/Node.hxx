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
 
#ifndef NODE_HXX
#define NODE_HXX

#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>

#include "ItPTree.hxx"
#include "RecodeToInt.hxx"
#include "Boolean.hpp"

using namespace std;

 

typedef vector< int > vectUI;


static int               mem =  0; // specify the way the policy memory allocation for the vectors                                    
                                   // >0 for "fixed" : the memory for the vectors is fixed (=mem)
                                   //       and freed only when the objet is deleted
    
static int globalOffset = 0 ;
    


//! Class representing a node of the PTree.
/**
   The template parameter Measure represents the type of the "bool vector" used to represents the presence or not of an element.
   For example bool to only store the presence or not of the element, int to store an integer measure (such as the support)...
   
   The template parameter SetsType represents the type of the elements inserted in the trie.   
   The template parameter Measure is eventually used to store additional informations associted with the sets stored.
*/
template <class SetsType, class Measure=Boolean>
class Node
{ 

    typedef vector<Measure> vectT;  
    typedef typename vector<Measure>::iterator vectTit;     
    
    typedef vector<Node *> vectN ;        
    typedef typename vector<Node *>::iterator vectNit ;      
    
    //! Functor used to insert an element to the current node
    class insertNode
    {
         public:
              
            Node< SetsType, Measure > * currentNode ;
            
            Node< SetsType, Measure > * precedentNode ;
            
            int precedentElem ;
            
            //! true if the set inserted was not in the trie  
            bool newset ;      
         
            //! length of the set  inserted        
            int length; 
                               
            insertNode( Node< SetsType, Measure > * inCurrNode, Node< SetsType, Measure > * inPrecNode=0 , int inPrtElem = -1 ) {  currentNode =  inCurrNode; precedentNode = inPrecNode ; precedentElem = inPrtElem; length = 0 ; newset=false; }
                                 
            void operator() ( int elem )
            {                       
                int index ;
                length = length + 1 ;                 

                if( currentNode ) // if there is a node
                {
                    precedentNode = currentNode;
                    precedentElem = elem ;                          
    
                    if( currentNode->cnts
                        && ( ( index = elem - currentNode->offset ) < currentNode->cnts->size() )            
                        && ( index >= 0 )
                        && currentNode->existVal( index ) 
                        ) // if the elem is in the node
                    {
                                    
                        if( currentNode->childs 
                           && ( index < currentNode->childs->size() )
                           && (*(currentNode->childs))[index] 
                           ) // this is not a leaf
                        {
                             currentNode = (*(currentNode->childs))[index] ;                                
                        }
                        else
                             currentNode = 0 ;                                                                                     
                    }
                    else // the node exist but the element is not in
                    {   
                                                
                         currentNode->addItVect( elem ) ;  // the element is inserted
                       
                         currentNode->cnts->at(elem -currentNode->offset )= Measure(true)   ;                          
                      
                         currentNode = 0 ;    
                         newset = true;                       
                    }    
                                                                   
                }
                else // the node must be created and link to the precedent one
                {
                      
                    if( mem > 0 )
                            currentNode = new Node< SetsType, Measure >(precedentElem, globalOffset,0,0,precedentNode ) ;
                    else
                            currentNode = new Node< SetsType, Measure >( precedentElem, elem,0,0,precedentNode );

                    currentNode->addItVect( elem ) ; // the element is inserted in the curent node

                    currentNode->cnts->at(elem -currentNode->offset )= Measure(true)  ;                            
                                    
                    index = precedentElem - precedentNode->offset ;

                    precedentNode->addItVect( precedentElem , currentNode ) ; // link the created node to the parent node

       	            precedentNode = currentNode;
       	            currentNode = 0 ;
                    precedentElem = elem ; 
                    
                    newset = true;

                }  
            }                                 
    };

protected:
    
    //! Parent node 
    Node *parent;                       
    
    //! Item  (internal id) in the parent node
    int id;          
    
    //! Offset of counter vector.
    int offset;      
  
    //! "bool vector"  used to represents the presence or not of an element.
    vectT * cnts;      
                  
    //! Pointer on the corresponding child nodes
    vectN * childs ;      
                 
public :

               
    //! Push back a set of element in the trie.
    /**
        This function corresponds to the classic push_back( ) function od STL container.
        The template parameter represents a container of integers.
        \param setElement the set of element to insert in the trie
        \param nb the number of times the set is inserted
        \return the size of the set inserted (if 0 the set was already in the trie)
    */             
    template< class Container >
    int push_back(  Container& setElement, Measure nb = Measure() )
    { 
        insertNode ins( this);

        ins = for_each( setElement.begin(), setElement.end(), ins  );       
        
        Node * lastElem = ins.precedentNode ;

//        (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] = (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] + nb ;;
        (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;

        if( ins.newset )
            return ins.length; 
        else
            return 0 ;
    }
         
    //! Push back a set of element in the trie.
    /**
        The template parameter represents the input iterators on the integers to insert int the trie.
        \param first iterator on the first element
        \param last iterator on the element after the last
        \param nb the number of times the set is inserted
        \return the size of the set inserted (if 0 the set was already in the trie)
    */             
    template< class InputIterator>
    int push_back( InputIterator & first, InputIterator & last, Measure nb =Measure() )
    {
        insertNode ins( this);      
        ins =for_each( first, last, ins  ); 
        
        Node * lastElem = ins.precedentNode ; 

        (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;
                                   
        if( ins.newset )
            return ins.length; 
        else
            return 0 ;
    } 
           
               
    
    //! Insert in child nodes to the current one.
    /**
       \param intid internal id of the current element in the node
       \param setElement the set of element to insert in the trie
       \param nb the number of times the set is inserted
       \return the size of the set inserted (if 0 the set was already in the trie)
    */
    template< class Container >           
    int insertChildNode(int intid, Container & setElement, Measure nb=Measure())           
    {
        int index;
   
        if( ( index = intid -offset) >= 0 && childs && index < childs->size() && (*childs)[index]  ) // if childs nodes exist
        {
            insertNode ins( (*childs)[index]);
            ins = for_each( setElement.begin(), setElement.end(), ins  );  

            Node * lastElem = ins.precedentNode ;

            (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;
                                              
            if( ins.newset )
                return ins.length; 
            else
                return 0 ;
        }        
        else // create a new leaf node
        {  
                    
            insertNode ins( 0,  this, intid); 
              
            ins = for_each( setElement.begin(), setElement.end(), ins  );       

            Node * lastElem = ins.precedentNode ; 

            (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;

            if( ins.newset )
                return ins.length; 
            else
                return 0 ;
        }
        
    }
    
    //! Insert in child nodes to the current one.
    /**
       \param intid internal id of the current element in the node
       \param first iterator on the first element
       \param last iterator on the element after the last
       \param nb the number of times the set is inserted
       \return the size of the set inserted (if 0 the set was already in the trie)
    */
    template< class InputIterator>           
    int insertChildNode(int intid, InputIterator & first, InputIterator & last, Measure nb = Measure())           
    {   
        int index;

        if( ( index = intid -offset) >= 0 &&childs && index < childs->size() && (*childs)[index]  ) // if childs nodes exist
        {
            insertNode ins( (*childs)[index]);
            ins = for_each( first, last, ins  );        

            Node * lastElem = ins.precedentNode ; 

            (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;

     
            if( ins.newset )
                return ins.length; 
            else
                return 0 ;
        }        
        else //  create a new leaf node
        {     

            insertNode ins( 0,  this, intid);
            ins = for_each( first, last, ins  );       
            
            Node * lastElem = ins.precedentNode ; 

            (*lastElem->cnts)[ ins.precedentElem - lastElem->offset ] =  nb ;

            if( ins.newset )
                return ins.length; 
            else
                return 0 ;
        }               
    }

    // -------------------------------------------------------------
    // method that delete the index-th item of the counters vector,
    // and modify the childs vector if necessary
    // ( the memory allocated to the vectors is reallocated wrt
    // the modifications)
  	// return the new Val for index (result +1 will be the new
    // current Val )
    // -------------------------------------------------------------
    
    int deleteItVect( int index ) ;    
    
    // --------------------------------------------------------------
    // method that allocate new memory in the counters vector and
    // modify the childs vector if necessary
    // item is the new item to insert in the cnts vector 
    // insChild is not null if we want to insert insChild as a child
    // of the new item being inserted 
    // sizecnts and sizechilds are the min size of vectors allocated
    // at creation
    // --------------------------------------------------------------
    
    void addItVect( int item,  Node * insChild=0, int sizecnts=1, int sizechilds = 1 ) ;
    
    

	// ------------
	// constructor
	// ------------

	Node( int inId=-1 ) : id(inId), offset(0), cnts(0), childs(0),parent(0){} //,succ(0) {}
	
	Node(int inId, int inOffset, vectT * inCnts=0,
         vectN * inChilds=0, Node *inParent=0, Node * inSucc=0 ):
         id(inId), offset(inOffset), cnts(inCnts), childs(inChilds), parent(inParent){} //, succ(inSucc) {} 
	
	// -----------
	// destructor
	// -----------

	~Node(){ if( cnts ) delete cnts; if( childs ) delete childs; }

	// ---------------------
	// recopy constructor
	// ---------------------

	Node(const Node & inNode) ;

	// ---------------------
	// affectation operator
	// ---------------------

	Node & operator = ( const Node & inNode ) ;

	// --------------------------------
	// method returning the attributes
	// --------------------------------
	
	int getId() const {return id;}
	
	vectT * getCnts(){ return cnts; }
	
	Measure getCnt(int i){ return cnts->at(i); }

	void setCnt(int i, Measure  incnt ){ (cnts->at(i)) = incnt; }

    bool existVal( int i ){ return  (cnts->at(i)) != Measure() ;}
           	
	vectN * getChilds(){ return childs ; } 
	
	int getChcnt() {return childs->size() ;}

	int getSize() { return cnts->size() ; }

	int getOffset() { return offset; }
	
	Node * getParent() { return parent; }
  
   	void setOffset( int inOffset ) { offset = inOffset; }
   	
   	Measure & getMeasure( int index ){ return (*cnts)[ index ]  ;}	
	
	// ------------------------------------------
	// method that allocates a set of child nodes
	// ------------------------------------------
		
	 void makeChildren( int inSize=1, int inChcnt=1 )
            { 
                if( !cnts ) { cnts = new vectT( inSize ) ; }    
                if(  ! childs ){ childs = new vectN( inChcnt ) ; }    
            }    
            
	// ---------------------------------------------------------
	// method that deletes recursively the sub tree of this node
	// ---------------------------------------------------------

	int deleteChildren();
	
	// ------------------------------
	// method that initializes the id 
	// ------------------------------

	void setId( int inId ) { id = inId ; }  

	// --------------------------------
	// method that initializes children 
	// --------------------------------

	void setCnts( vectT * inCnts ) { cnts = inCnts ; }  

	// ------------------------------------------------------
	// method which insert an itemset in the sub tree rooted
    // by the current node
    // return the les node inserted
    // ------------------------------------------------------

	Node * insert( int * itemset, int level, Measure inSup = Measure(true) ) ;

	Node * insert( vectUI * itemset, Measure inSup = Measure(true) ) ;

	Node * insert( list< int > * itemset, Measure inSup = Measure(true) ) ;
	
	// ------------------------------------------------------
	// method which delete an itemset in this set of itemset
	// rooted by the node ( all necessary mem reallocation 
	// are did to keept the smallest representative trie)
	// ------------------------------------------------------

	bool deleteIt( vectUI * itemset )
        { int pos = 0;  return deleteIt( itemset, pos ) ; } 
	
	bool deleteIt( vectUI * itemset, int & pos ) ;
		
    // ---------------------------------------------------
	// print to screen all the itemset stored in sub tree
	// remap is a table for mapping the name of the items
	// if remap is 0 it use the internal id
	// ---------------------------------------------------

	void print( RecodeToInt<SetsType> * recode = 0 ) ;

	void print( RecodeToInt<SetsType> * recode, vectUI * vect ) ;
	
	// ----------------------------------------------------
    // print to screen all the supersets stored in sub tree
	// (ie in the leaf node)
	// recode is a functor for mapping the name of the items
	// ----------------------------------------------------
    
    void printSupersets( RecodeToInt<SetsType> * recode = 0 ) ;

	void printSupersets( RecodeToInt<SetsType> * recode, vectUI * vect ) ;
	
	// ---------------------------------------------------
	// read from a file all the itemset and stored them 
	// into a prefix tree
	// ---------------------------------------------------

	int readItemsets( const char * fileName, int * remap = 0 ) ;
	
	// ---------------------------------------------------
	// save into a file all the itemset stored in tree
	// remap is a table for mapping the name of the items
	// if remap is 0 it use the internal id
	// ---------------------------------------------------

	void saveItemsets( const char * fileName, int * remap = 0 ) ;

	void saveItemsets( ofstream & pFile, int * remap , vectUI * vect) ;
	
	void save( const char * fileName, RecodeToInt<SetsType> * recode = 0 ) ;

	void save( ofstream & pFile, RecodeToInt<SetsType> * recode  , vectUI * vect) ;

    // ---------------------------------------------------
	// save into a file all the itemset stored in tree
	// remap is a table for mapping the name of the items
	// according the FIMI format of datasets 
	// ---------------------------------------------------

	void saveDataSet( const char * fileName, int * remap=0 ) ;

	void saveDataSet( ofstream & pFile, int * remap , vectUI * vect) ;
	
	// -----------------------------------
	// method searching if one itemset of 
	// the sub trees from inCnts is 
    // included into itemset
	// -----------------------------------

	bool includedIn( int * itemset, int level, int spos = 0 ) ;
	
	bool includedIn(  vectUI * itemset, int spos = 0 ) ;

	bool includedIn( list< int > * itemset, list< int >::iterator spos ) ;
	
	// ----------------------------------------------
	// method searching if itemset is include in one
	// of the itemsets of this set of itemset
	// ----------------------------------------------
	
	bool include( vectUI * itemset, int spos = 0 ) ;

	bool include( int * itemset, int level, int spos = 0 ) ;
	
	// -------------------------------------------------
	// method merging this sets of itemset 
	// and Node2 into this set 
	// note: the set Node2 is destructed after the copy
	// -------------------------------------------------

	void merge( Node * node2 ) ;

	// -----------------------------------------------------------------
	// method that construct the complementary of this set of itemset
	// ( res ) since we have all the items ordered and with the internal
	// id begining at 1 to the number of items in the database
	// we only have to use the total number of items to find the
	// complementary
	// -----------------------------------------------------------------
    
	Node * complem( vectUI & listElem, int & maxSize  ) ;

	void complRec( vectUI & listElem, vectUI * vect, Node * res, int & maxSize ) ; 

	// -------------------------------------
	// method that proces the complement of 
	// an itemset in whom the items are
	// between 1 an nb
	// -------------------------------------

	vectUI * complem( vectUI * vect,  vectUI & listElem ) ;
	
	// ------------------------------------------
	// method that generate into this set 
	// the subsets of size "size" of an itemsets 
	// Note: items must be already alocated
	// -------------------------------------------

	int genSubsets(  vectUI * itemset, int size )
	{ return genSubsets( itemset,  0, itemset->size() - size ); }

	int genSubsets(  vectUI * itemset, int spos , int depth) ;

	// ------------------------------------------------
	// method that return a set of the subsets of size
	// "size" of this set of itemsets 
	// ------------------------------------------------

	Node * genSubsets( int size ) ;

	void genSubsets( int size, vectUI * itemset, Node * subset ) ;

 friend class ItPTree<SetsType, Measure>;
 friend class PTree<SetsType, Measure>;
};

//------------------------------------------------------------------------------

/**
*  method that delete the index-th item of the counters vector,
*  and modify the childs vector if necessary
*  ( the memory allocated to the vectors is reallocated wrt
*  the modifications)
*  return the current value of index (result +1 will be the new
*  current value )
*/

template<class SetsType,class Measure>     
int Node< SetsType, Measure >::deleteItVect( int  index )
{
    int i;

    vectTit itcnts ;
    
    vectNit itchilds ;


    if( cnts->size() == 1 ) // this the only item in the node
    {
         
         delete cnts ;

         cnts = 0 ;

         if( childs )
         {
              if( (*childs)[0]  )
                  (*childs)[0]->deleteChildren() ;                 

              delete (*childs)[0] ;
              (*childs)[0] = 0 ;
              
              delete childs ;
              childs = 0 ;
         }   
                                              
    }    
    else    
    {
           
         setCnt( index , Measure() ) ;
         
         if( index == cnts->size()-1 ) // this is the last item of the vector
         {                             // we erase the end of the vector    
                  i = index - 1 ;
                             
                  while( ( i >= 0 ) && ( ! existVal( i ) ) )
                       i-- ;            
                                                                                                                                                          
                  cnts->resize( i+1 ); // we delete the last empty elements                                           

                  if( childs && index < childs->size()  )
                  {
                        if( (*childs)[index] )
                                 (*childs)[index]->deleteChildren() ;
                                 
                        delete (*childs)[index] ;
                        (*childs)[index] = 0 ;                            
                                            
                        while( ( i >= 0 ) && (  ! (*childs)[ i ] ) )                         
                                 i-- ;
                            
                        if( i < 0)
                        {
                            delete childs;
                            childs = 0 ;
                         }  
                         else  
                            childs->resize( i+1 ) ;

                    }               

                    if( cnts->size() == 0 )
                    {
                        delete cnts ;
                        cnts = 0 ;
                    }               
                        
           }
          else if( index == 0 ) // this is the first item 
           {                     // we erase the begining of the vector       

                itcnts = cnts->begin() ;
           		itcnts++ ;          		

                // search of the new first element
           		i=1 ;         		
           		while( ( itcnts != cnts->end() ) && (  *itcnts == Measure() ) )
           		{
                      itcnts++ ;
                      i++ ;
                }    

                cnts->erase( cnts->begin() , itcnts ) ;  // erase the unused elements of cnts                       

                if( cnts->size() == 0 )
                {
                    delete cnts ;
                    cnts = 0 ;
                }   
                
               if( childs )
               {      

                        if( (*childs)[0] )
                        {
                           
                                (*childs)[0]->deleteChildren() ;
                        }    

                        delete (*childs)[0] ;
                        (*childs)[0] = 0 ;      
                        
                        if( childs->size() == 1 ) // if this the only element in childs
                        {
                                delete childs ;
                                childs = 0 ;
                        }
                        else
                        {
                                itchilds = childs->begin() ;

                                if( i >= childs->size() )
                                    itchilds= childs->end() ;
                                else
                                    itchilds = itchilds+ i;                                
                                                                  
                                childs->erase( childs->begin() , itchilds ) ; // erase the unused elements of childs                        

                                if( childs->size() == 0 )
                                {
                                    delete childs ;
                                    childs = 0 ;
                                }
                                                                                                                                                          
                        }                                    
                }                        
                offset = offset + i ; // new offset
     	} 	
        else if( childs && index < childs->size() )
     	{
     	       if( (* childs)[ index ] )
     	       {
     	                    (* childs)[ index ]->deleteChildren() ;
     	                    (* childs)[ index ] = 0 ;
               }    
     	                    
     	}    

   	}  
  
    // if index is used in a loop
    // index has to be set to -1
    // (the next new element tested will be the first ie 0)          

    if( index == 0 )    index = -1 ;          

    return index ;    
}       

// ----------------------------------------------------------------------------------------------   

/**
*  method that allocate new memory in the counters vector and
*  modify the childs vector if necessary
*  item is the new item to insert in the cnts vector 
*  insChild is not null if we want to insert insChild as a child
*  of the new item being inserted 
*  sizecnts and sizechilds are the min size of vectors allocated
*  at creation
*/
template<class SetsType,class Measure>         
void Node< SetsType, Measure >::addItVect( int item, Node * insChild, int sizecnts, int sizechilds )
{
    vectT * tmpcnts ;
    vectN * tmpchilds ;
	
	int index = item - offset ;


    if( cnts == 0 )
    {
        if( mem > 0)
        {
            cnts = new vectT( mem, Measure() ) ; 
          
            offset = globalOffset ;
        }    
        else
        {     
             
            cnts = new vectT( sizecnts, Measure() ) ;   

           
            offset = item ;
         }    
         
    } 
    else
    {           
  
        if( index < 0) // reconstruct the two vectors with the new item in first
    	{	    
    	    tmpcnts = new vectT( cnts->size() - index, Measure() ); // we allocate a new vector
     	                                      // begining at the new offset  
                                              // itemset[i]  
                       
            for( int m = 0; m < cnts->size() ; m++ )     // recopy  old Vals                         
                    (*tmpcnts)[ offset - item + m ] = (*cnts)[ m ] ;
               
            delete cnts ;
            
            cnts = tmpcnts ;
    
            // we reconstruct the child vector (if exists) to have a correct matching 
            // between the counters and the childs
            
            if( childs )
            {
                tmpchilds = new vectN( childs->size() - index,0 ) ;
                    
                for( int m = 0; m < childs->size() ; m++ ) // recopy old Vals
                    (*tmpchilds)[ offset - item + m ] = (*childs)[ m ] ;
                    
                delete childs ;
                
                childs = tmpchilds ;
                
            }    
             
            offset = item ;  
            
            index = 0 ;               	    
    	   
    	}
    	else if( index >= cnts->size() )   // we have to add a new last item 
    	{
    
             cnts->resize( index+1 , Measure() ) ;        	              
         	              
    	}
    }   	
    
    index = item - offset ;
  
    if( ! existVal( index ) ) 
    {     
       
        cnts->at(index) = Measure(true); // setCnt( index, 1 );    // the item is inserted 
    }

    if( insChild )
    { 
        if( childs == 0 )
        {
                 if( mem > 0)
                 {
                     childs = new vectN(  mem, (Node*)0 ) ;      
                                                                  
                 }    
                 else
                 {
                     if( index +1 > sizechilds )
                          childs = new vectN( index +1 ,(Node*)0) ; 
                     else 

                          childs = new vectN( sizechilds,(Node*)0 ) ; 
                 }        


        }    
        else if( childs->size() <= index )
        {
                childs->resize(  index+1  , 0 ) ;
        }

        // link with a new child node
                    
        ( *childs )[ index ] =  insChild  ; 
    }  
   
}    

// ---------------------------------------------------------------------------------------------- 
    
/**
*  Recopy Constructor
*/
template<class SetsType,class Measure>     
Node< SetsType, Measure >::Node(const Node & inNode) 
{
	id =  inNode.id ;

	offset = inNode.offset ;
	
	if( inNode.cnts )
	{ 
		*cnts = * inNode.cnts ;
		
    }
    else
        cnts = 0 ;
        
                
    if( inNode.childs )
	{ 
		*childs = *inNode.childs ;		  
    }
    else
        childs = 0 ;           		  

	parent = inNode.parent ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  affectation operator
*/
template<class SetsType,class Measure>     
Node< SetsType, Measure > & Node< SetsType, Measure >::operator = ( const Node< SetsType, Measure > & inNode ) 
{
	if( this != &inNode )
	{
		if( cnts ) 
			delete cnts ;
			
		if( childs )
            delete childs ;	
            
		id =  inNode.id ;

		offset = inNode.offset ;
		
		if( inNode.cnts )
    	{ 
	    
    		*cnts =  *inNode.cnts ;		  
        }
        else
            cnts = 0 ;
            
                    
        if( inNode.childs )
    	{ 
    		*childs =  *inNode.childs ;		  
        }
        else
            childs = 0 ;        
         		  
    	parent = inNode.parent ;

   	
	}

	return *this ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that delete recursively the sub Node of this node
*/
template<class SetsType,class Measure>     
int Node< SetsType, Measure >::deleteChildren() 
{
	int deleted=0;

	if( childs )
	{
		for( int i = 0; i < childs->size() ; i++ )
		{
            if( (*childs)[i] )
            {	                    
                deleted += (*childs)[i]->deleteChildren();

                delete ( (*childs)[i] ) ;

                (*childs)[i] = 0 ;
            }    

		}
		delete childs ;
		childs = 0 ;
		
		delete cnts ;
		cnts = 0 ;
		

	}
	else
	{
	    if( cnts )
	    {
	        delete cnts ;
	        cnts = 0 ;
	    }    
	}    
    
	deleted++;
                        
	return deleted;
}    

// ---------------------------------------------------------------------------------------------- 

/**
*  method which insert an itemset in the sub tree rooted by the current node
*  sizeAlloc determine the size allocated (reserved) to 
*  the childs at initilisation vector. If = to 0 the 
*  allocation is dynamically made when a new inserted 
*  item imply to enlarge the vector ( new memory 
*  allocation and recopy of the olds Vals are made)
*/
template<class SetsType,class Measure>     
Node< SetsType, Measure > * Node< SetsType, Measure >::insert( int * itemset, int level, Measure inSup) 
{
    
    vectN * currChilds ;
	
	Node< SetsType, Measure > * currNode = this ;
    Node< SetsType, Measure > * tmpCh ;
	
	int i = 0 ;	
	
	int index ;

	currChilds = childs ;

	// we pass all the items of the itemset to insert
	// that are already stored in the subtree 
	// as long as there is child nodes
	
	while( ( i<level ) &&  currNode->cnts
           && ( ( index = itemset[ i ] - currNode->offset ) < currNode->cnts->size() )            
           && ( index >= 0 )
           && currNode->existVal( index ) 
           && currChilds 
           && ( index < currChilds->size() )
           && (*currChilds)[index] 
           )			
	{  
   
	   if( i == level -1 ) // a super set of the itemset is already in the sub tree
              return 0 ; 
                                                                                                                                                                                                                          
       currNode = (*currChilds)[index] ;
       currChilds = currNode->childs ;
                
       i++ ;		
	}
	
   // insertion of the part of the itemset not already stored in itemset

	for( int j = i; j < level-1; j++ )
	{
        index = itemset[ j ] - currNode->offset ;

        if( mem > 0 )
                tmpCh = new Node< SetsType, Measure >( itemset[ j ], globalOffset,0,0,currNode ) ; //, new vectUI(mem),0 ) ;
        else
                tmpCh = new Node< SetsType, Measure >( itemset[ j ], itemset[ j+1 ],0,0,currNode ) ;//, new vectUI(1),0 ) ;

	    currNode->addItVect( itemset[ j ], tmpCh  ) ;
	
        currNode = tmpCh ;
        
    }
    
    // the last item is inserted	
	
	currNode->addItVect( itemset[ level-1 ], 0  ) ;
	
	index = itemset[ level-1 ] - currNode->offset ;

	currNode->setCnt( index, inSup ) ; 	
 
    return currNode ;               

}

template<class SetsType,class Measure>     
Node< SetsType, Measure > * Node< SetsType, Measure >::insert( vector< int > * itemset, Measure inSup ) 
{
    
    vectN * currChilds ;
	
	Node< SetsType, Measure > * currNode = this ;
	Node< SetsType, Measure > * tmpCh ;
	
	int i = 0 ;	
	
	int index ;
	currChilds = childs ;

	// we pass all the items of the itemset to insert
	// that are already stored in the subtree 
	// as long as there is child nodes

	while( ( i < itemset->size() ) && currNode->cnts
           && ( ( index =  (*itemset)[i ] - currNode->offset ) < currNode->cnts->size() )            
           && ( index >= 0 )
           && currNode->existVal( index ) 
           && currChilds 
           && ( index < currChilds->size() )
           && (*currChilds)[index] 
           )			
	{  
   
	   if( i == itemset->size() -1 ) // a super set of the itemset is already in the sub tree
              return 0 ; 
                                                                                                                                                                                                                          
       currNode = (*currChilds)[index] ;

       currChilds = currNode->childs ;
                
       i++ ;		
	}

	// insertion of the part of the itemset not already stored in itemset

	for( int j = i; j < itemset->size()-1; j++ )
	{
        index = (*itemset)[ j ] - currNode->offset ;

        if( mem > 0 )
                tmpCh = new Node< SetsType, Measure >( (*itemset)[ j ], globalOffset,0,0,currNode) ;
        else                
                tmpCh = new Node< SetsType, Measure >( (*itemset)[ j ], (*itemset)[ j+1 ],0,0,currNode);

	    currNode->addItVect( (*itemset)[ j ], tmpCh  ) ;
	
        currNode = tmpCh ;
        
    }
	   
    // the last item is inserted	
	
	currNode->addItVect( (*itemset)[ itemset->size()-1 ], 0  ) ;
	
	index = (*itemset)[ itemset->size()-1 ] - currNode->offset ;

	currNode->setCnt( index, inSup ) ;
    return currNode ;               

}

template<class SetsType,class Measure>     
Node< SetsType, Measure > * Node< SetsType, Measure >::insert( list< int > * itemset, Measure inSup ) 
{
   
    vectN * currChilds ;
	
	Node< SetsType, Measure > * currNode = this ;
	Node< SetsType, Measure > * tmpCh ;
	
	int i = 0 ;	
	
	int index ;
	
	list< int >::iterator it;

	it = itemset->begin() ;

	currChilds = childs ;

	// we pass all the items of the itemset to insert
	// that are already stored in the subtree 
	// as long as there is child nodes
	
	while( ( it != itemset->end() ) && currNode->cnts
           && ( ( index =  *it - currNode->offset ) < currNode->cnts->size() )            
           && ( index >= 0 )
           && currNode->existVal( index ) 
           && currChilds 
           && ( index < currChilds->size() )
           && (*currChilds)[index] 
           )			
	{  
   
	   if( i == itemset->size() -1 ) // a super set of the itemset is already in the sub tree
              return 0 ; 
                                                                                                                                                                                                                          
       currNode = (*currChilds)[index] ;

       currChilds = currNode->childs ;
                
       i++ ;
       it++ ;		
	}

	for( int j = i; j < itemset->size()-1; j++ )
	{
	    
        index = *it - currNode->offset ;

        int item = *it ;
        if( mem > 0 )
        {
                tmpCh = new Node< SetsType, Measure >( item, globalOffset,0,0,currNode); //, new vectUI(mem), 0 ) ;
                it++;
        }    
        else
        {

                tmpCh = new Node< SetsType, Measure >( item, *it,0,0,currNode) ;//, new vectUI(1), 0 ) ;

                it++ ;

        }    
                              

	    currNode->addItVect( item, tmpCh  ) ;
        currNode = tmpCh ;
         
    }
    // the last item is inserted	
	
	currNode->addItVect( *it, 0  ) ;
	index = *it - currNode->offset ;

	currNode->setCnt( index, inSup );	
 
 
    return currNode ;               
    
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method which delete an itemset in a set of itemset
*/
template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::deleteIt( vectUI * itemset, int & pos ) 
{
    int index ;
    int i = 0 ;
    bool res = false ;
    
	
	if( pos < itemset->size() )
	{
	    index = (*itemset)[ pos ] - offset ;
	
		// we search for the current item (pos+1-th item) of "itemset"
			
		if( index >=0 && index < cnts->size() && existVal( index ) )
		{
		    if( childs && ( index < childs->size() ) && (*childs)[ index ] )
            {
                 pos++ ;
                 res = (*childs)[ index ]->deleteIt( itemset, pos ) ;
                 
                 if( (*childs)[ index ]->cnts  == 0 ) // there is no more item in the child node
			     {
			           delete ( (*childs)[ index ] ) ;
			           (*childs)[ index ] = 0 ;
			     }    
            }
            else
                 pos++ ;     
                                                                                            
                                    
			// we delete the item if there is no more child nodes for this item
			if( ( pos == itemset->size() ) // if we have found the itemset
                 && ( !childs  || ( index >= childs->size() ) || !(*childs)[ index ] ) 
                                  // and is not prefix of another itemset
                      || ! (*childs)[ index ]->cnts  ) 
			{			    			    
			    deleteItVect( index ) ;   
                
                res = true ;                 
            }    			

		}

	}

	return res ;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  print to screen all the itemset stored in the sub tree
*  remap is a table for mapping the name of the items
*/
template<class SetsType,class Measure>     
void Node< SetsType, Measure >::print( RecodeToInt<SetsType> * recode )  
{
	vectUI * vect ;

	vect = new vectUI ;

	if( cnts != 0 )
		print( recode, vect ) ;
	
	delete vect ;
}

template<class SetsType,class Measure>     
void Node< SetsType, Measure >::print( RecodeToInt<SetsType> * recode,  vector< int > * vect ) 
{
    Measure inSup ;

	for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;    		     	
   			
   			// we continue searching with the next items
 
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
               
                if(  (inSup = getCnt( i )  ) != Measure(true) ) //check if there is a subset
                {
                    for( int j = 0; j < vect->size() ; j++ ) // print a subset
               		{
               			if(recode)  
               				cout<< recode->remap( (*vect)[ j ] )<<" " ; 
               			else 
               				cout<< (*vect)[ j ]<<" " ;  
               	
               		}
               		
                    cout<<"  "<< inSup<<endl ;                                        
                }
                
                // process next node
       		    (*childs)[ i ]->print( recode,  vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		     // so we print to screen
       		{
                inSup = getCnt( i ) ; //number of i element  in the node
         
       		    for( int j = 0; j < vect->size() ; j++ )
           		{
           			if(recode)  
           				cout<< recode->remap( (*vect)[ j ] )<<" " ; 
           			else 
           				cout<< (*vect)[ j ]<<" " ;  
           	
           		}
           		cout<<"  : "<< inSup<<endl ;         		    
       		}    
                                
   			vect->pop_back() ;
           }
				
	}
}

// ---------------------------------------------------------------------------------------------- 

// ----------------------------------------------------
// print to screen all the supersets stored in sub tree
// (ie in the leaf node)
// recode is a functor for mapping the name of the items
// ----------------------------------------------------
template<class SetsType,class Measure>     
void Node< SetsType, Measure >::printSupersets( RecodeToInt<SetsType> * recode )  
{
	vectUI * vect ;

	vect = new vectUI ;

	if( cnts != 0 )
		printSupersets( recode, vect ) ;
	
	delete vect ;
}

template<class SetsType,class Measure>     
void Node< SetsType, Measure >::printSupersets( RecodeToInt<SetsType> * recode,  vector< int > * vect ) 
{
    int inSup ;

	for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;
    		
       	
   			
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
                // process next node
       		    (*childs)[ i ]->printSupersets( recode,  vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		     // so we print to screen
       		{
                inSup = getCnt( i ) ; //number of i element  in the node
    
     
       		    for( int j = 0; j < vect->size() ; j++ )
           		{
           			if(recode)  
           				cout<< recode->remap( (*vect)[ j ] )<<" " ; 
           			else 
           				cout<< (*vect)[ j ]<<" " ;  
           	
           		}
           		cout<<"  : "<< inSup<<endl ;         		    
       		}    
                                
   			vect->pop_back() ;
           }
				
	}
}

// ---------------------------------------------------------------------------------------------- 

/**
*  read from a file all the itemset and stored them 
*  into a prefix tree
*/
template<class SetsType,class Measure>     
int Node< SetsType, Measure >::readItemsets( const char * fileName, int * remap  )
{


    int nb = 0 ; // number of itemsets inserted
    
    list< int > * itemset ;
    FILE * f = 0;
    f = fopen(fileName,"r");  
  
    if ( f == 0 )
    {
         cout<<"File cannot be read!";
         exit(1);
    }

    while( !feof(f)   )
    {
        itemset = new list< int >;                                                       
        char c;   
        
        do
        {
            int item=0, pos=0;
            c = getc(f);
                           
            while( ( c >= '0' ) && ( c <= '9' ) )
            {
                  item *=10;
                  item += int(c)-int('0');
                  c = getc(f);
                  pos++;
            }
            
            if(pos)
            {         
                  itemset->push_back( item );                                    
            }   
             
        }while( c != '\n' && !feof( f ) );
               
        if( itemset->size() != 0 )		
        {
        	if( insert( itemset ) )
                nb++ ;
        	
       	}   	
            
        delete itemset ;
    }    
    
    
    fclose( f ) ;
    
    return nb ;
    
}    


// ---------------------------------------------------------------------------------------------- 

/**
*  save into a file all the itemset stored in tree
*  remap is a table for mapping the name of the items
*  if remap is 0 it use the internal id
*/
template<class SetsType,class Measure>     
void Node< SetsType, Measure >::save( const char * fileName, RecodeToInt<SetsType> * recode ) 
{

	ofstream pFile( fileName ) ;

	vectUI * vect ;

	vect = new vectUI ;

	if( pFile.is_open() )
	{
		if( cnts != 0 )
			save( pFile, recode, vect) ;

		pFile.close()  ;
	}

	delete vect ;

}

template<class SetsType,class Measure>     
void Node< SetsType, Measure >::save( ofstream & pFile, RecodeToInt<SetsType> * recode, vectUI * vect ) 
{
    int inSup ;
    Measure val;  
    
    for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;
    			
  			inSup = getMeasure( i )    ;  
  
    		val = ( *cnts)[i] ;  
    
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
       		    (*childs)[ i ]->save( pFile, recode,  vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		     // so we print to screen
       		{
                         
       		    for( int j = 0; j < vect->size() ; j++ )
        		{
        			if(recode) 
        			{   
                    	pFile<< recode->remap( (*vect)[ j ] )<<" " ; 
                    }
        			else 
        				pFile<< (*vect)[ j ] <<" " ; 
        	
        		}

                                                       			
        		pFile<<"("<< inSup<<")"<<endl;
       		}    
                                
   			vect->pop_back() ;
           }
				
	}

}

// ---------------------------------------------------------------------------------------------- 


/**
*  save into a file all the itemset stored in tree
*  remap is a table for mapping the name of the items
*  if remap is 0 it use the internal id
*/
template<class SetsType,class Measure>     
void Node< SetsType, Measure >::saveItemsets( const char * fileName, int * remap ) 
{

	ofstream pFile( fileName ) ;

	vectUI * vect ;

	vect = new vectUI ;

	if( pFile.is_open() )
	{
		if( cnts != 0 )
			saveItemsets( pFile, remap, vect) ;

		pFile.close()  ;
	}

	delete vect ;

}

template<class SetsType,class Measure>     
void Node< SetsType, Measure >::saveItemsets( ofstream & pFile, int * remap, vectUI * vect ) 
{
    int inSup ;
    Measure val;  
    
    for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;
    			
  			inSup = getMeasure( i )    ;  
  
    		val = ( *cnts)[i] ;  
    
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
       		    (*childs)[ i ]->saveItemsets( pFile, remap,  vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		     // so we print to screen
       		{
       		    for( int j = 0; j < vect->size() ; j++ )
        		{
        			if(remap) 
        			{   
                    	pFile<< remap[ (*vect)[ j ] ]<<" " ; 
                    }
        			else 
        				pFile<< (*vect)[ j ] <<" " ; 
        	
        		}

                                                       			
        		pFile<<"("<< inSup<<")"<<endl;
       		}    
                                
   			vect->pop_back() ;
           }
				
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  save into a file all the itemset stored in tree
*  remap is a table for mapping the name of the items
*  according the FIMI format of datasets 
*/
template<class SetsType,class Measure>     
void Node< SetsType, Measure >::saveDataSet( const char * fileName, int * remap ) 
{

	ofstream pFile( fileName ) ;

	vectUI * vect ;

	vect = new vectUI ;

	if( pFile.is_open() )
	{
		if( cnts != 0 )
			saveDataSet( pFile, remap, vect) ;

		pFile.close()  ;
	}

	delete vect ;

}

template<class SetsType,class Measure>     
void Node< SetsType, Measure >::saveDataSet( ofstream & pFile, int * remap, vectUI * vect ) 
{
    
    for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;
    			    
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
       		    (*childs)[ i ]->saveDataSet( pFile, remap,  vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		     // so we print to screen
       		{
       		    for( int j = 0; j < vect->size() -1 ; j++ )
        		{
        			if(remap) 
        				pFile<< remap[ (*vect)[ j ] ]<<" " ; 
        			else 
        				pFile<< (*vect)[ j ] <<" " ; 
        	
        		}
        		
       			if(remap) 
        			 pFile<< remap[ (*vect)[ vect->size() -1 ] ]<<endl ; 
        		else 
        		     pFile<<(*vect)[ vect->size() -1 ]<<endl;
                 		    
       		}    
                                
   			vect->pop_back() ;
           }
				
	}

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method searching if one itemset of 
*  of the sub tree rooted by this node
*  is included into "itemset"
*  be carefull itemset MUST BE ordered
*/
template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::includedIn( int * itemset, int level, int spos ) 
{
	bool res = false ; // result of the inclusion test
	
	
	int index = 0;
	
    if( spos < level )
    {
        // we search an item in "itemset"
        // that is >= of the first item 
        // stored in the node 
        	
    	index  = itemset[spos] - offset ;

    	while( index < 0   )
    	{
            spos ++ ; 
            
            if ( spos >= level ) return res;
             	
            index  = itemset[spos] - offset ;
        }    

    	while( ( res == false ) &&  cnts && ( index < cnts->size() )  )
    	{          
                
    	    if( existVal( index ) )     // the index-th item in the node is in "itemset"
    	    {

    	    	if( (childs) && ( index < childs->size() ) && (*childs)[ index ] )
     	    	       res = ( (*childs)[ index ] )->includedIn( itemset, level, spos+1 ) ;        		   
              	else  
               	       // we have check all the item of the current itemset
               	       // of this set       
                       res = true ;
            }
            
            spos++ ;
          
            if( spos <level )
                index  = itemset[spos] - offset ;
            else
                return res;              
                
          }
   	}
   	
	return res ;

}

template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::includedIn( vector< int > * itemset, int spos ) 
{
    
    bool res = false ; // result of the inclusion test
	
	
	int index = 0;
	
    if( spos < itemset->size() )
    {
        // we search an item in "itemset"
        // that is >= of the first item 
        // stored in the node 
        	
    	index  = (*itemset)[spos] - offset ;
    	
    	while( index < 0   )
    	{
            spos ++ ; 
            
            if ( spos >= itemset->size() )
               return res;
             	
            index  = (*itemset)[spos] - offset ;
        }    
        	    	    	
    	while( ( res == false ) &&  cnts && ( index < cnts->size() )  )
    	{
      
    	    if( existVal( index ) ) // the index-th item in the node is in "itemset"
    	    {
        		if( (childs) && ( index < childs->size() ) && (*childs)[ index ] )
        		       res = ( (*childs)[ index ] )->includedIn( itemset, spos+1 ) ;
              	else  
               	       // we have check all the item of the current itemset
               	       // of this set       
                       res = true ;
            }
            
            spos++ ;
            
            if( spos < itemset->size() )
                index  = (*itemset)[spos] - offset ;
            else
                return res;
            
            
       	}
   	}
   	
	return res ;

}

template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::includedIn(  list< int > * itemset, list< int >::iterator spos ) 
{
    
    bool res = false ; // result of the inclusion test
	
	list< int >::iterator tmp ;
	
	int index = 0;

    if( spos != itemset->end() )
    {
        // we search an item in "itemset"
        // that is >= of the first item 
        // stored in the node 
        	
    	index  = *spos - offset ;

    	while( index < 0   )
    	{
            spos ++ ; 

            if ( spos ==  itemset->end() )
               return res;
             	
            index  = *spos - offset ;

        }    
       	    	    	
    	while( ( res == false ) &&  cnts  &&( index < cnts->size() )  )
    	{
    	    
    	    if( existVal( index ) ) // the index-th item in the node is in "itemset"
    	    {
        		if( childs && ( index < childs->size() ) && (*childs)[ index ] )
        		{
                       tmp = spos ;
        		       res = ( (*childs)[ index ] )->includedIn( itemset, ++tmp ) ;
     		    }    
              	else  
               	       // we have check all the item of the current itemset
               	       // of this set       
                       res = true ;
            }
            
            spos++ ;
           
            if( spos != itemset->end() )
                index  = *spos - offset ;
            else
                return res;
            
            
       	}

   	}

	return res ;
	
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method searching if itemset is include in one
*  of the itemsets of this set of itemset
*/
template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::include( vectUI * itemset, int spos ) 
{

	bool res = false ; // result of the inclusion test

	int i = 0;
	int index  ;
	int maxi =0 ;

	if( spos < itemset->size() )
    {
	    index  = (*itemset)[spos] - offset ;

	    if( index < 0  ) // the item is before all the current items of the tree
	         return false ;

	    // test if the id of the current element
    	// is equal to an item of the
    	// cuurent node
    	// and if equal we check the next item of the current
    	// itemset of this set 
                        
        if( ( index < cnts->size() ) && existVal(  index ) )
        { 
                if( (childs) && ( index < childs->size() ) && (*childs)[ index ] )
        		       res = ( (*childs)[ index ] )->include( itemset, spos+1 ) ;              	
				else  
				       // we have check all the item of the current itemset
               	       // of this set       
                       if( spos == itemset->size()-1 ) 
                                  return true ;    
		}

        // explore all itemsets that have current node items smaller than
        // the current item of "itemset" 
        
        if( res == false )    
        {		
    	    if( index  > cnts->size() )     // we define the max index for the counter vector
    	            maxi = cnts->size() ;
            else
                    maxi = index ;
                        	                     
    		while( ( res == false ) && ( i < maxi )  )
    		{ 				    			    		    
    			if( (childs) && ( i < childs->size() ) && (*childs)[ i ] )
            		       res = ( (*childs)[ i ] )->include( itemset, spos ) ;              	
    						
    			i++ ;
    
    		}
        }  		

	}
	else
	    if( spos == itemset->size() )
           return true ;
	
	return res ;
}

template<class SetsType,class Measure>     
bool Node< SetsType, Measure >::include( int * itemset, int level, int spos ) 
{
	bool res = false ; // result of the inclusion test

	int i = 0;
	int index  ;
	int maxi =0 ;

	if( spos < level )
    {
	    index  = itemset[spos] - offset ;
	    
	    if( index < 0  ) // the item is before all the current items of the tree
	         return false ;

	    // test if the id of the current element
    	// is equal to the current to the
    	// cuurent element of the itemset 
    	// and we check the next item of the current
    	// itemset of this set 
            
        if( ( index < cnts->size() ) && existVal( index ) )
        { 
                if( (childs) && ( index < childs->size() ) && (*childs)[ index ] )
        		       res = ( (*childs)[ index ] )->include( itemset, level, spos+1 ) ;              	
				else  
				       // we have check all the item of the current itemset
               	       // of this set       
                       if( spos == level-1 ) 
                                  return true ;    
		}
		
        // explore all itemsets that have current node items smaller than
        // the current item of "itemset" 
        
        if( res == false )    
        {			
    	    if( index  > cnts->size() )     // we define the max index for the counter vector
    	            maxi = cnts->size() ;
            else
                    maxi = index ;
    	                     
    		while( ( res == false ) && ( i < maxi )  )
    		{ 				    			
    			if( (childs) && ( i < childs->size() ) && (*childs)[ i ] )
            		       res = ( (*childs)[ i ] )->include( itemset, level, spos ) ;              	
    						
    			i++ ;
    
    		}
        }  		

	}
	else
	    if( spos == level )
           return true ;
	
	return res ;
}

// ---------------------------------------------------------------------------------------------- 

/** 
*  method merging this sets of itemset 
*  and Node2 into this set 
*  note: the set Node2 is destructed after the copy
*/
template<class SetsType,class Measure>
void Node< SetsType, Measure >::merge( Node< SetsType, Measure > * node2 ) 
{	
    int index ; // index of node2 converted for this trie since the offset
                // could be different 
    
	// we go throw the tree node2 to an item that is not stored in this trie

	int i= 0 ;

	if( cnts )
	{
        while( node2->cnts && i < node2->cnts->size() )
    	{

    	    if( node2->existVal( i ) )
    	    {	     
                index = node2->offset + i - offset ;  

                // we check if the current item is already in this trie
                
                if( index >= 0 && index < cnts->size() && existVal( index ) )
                {
              
                   if( node2->childs && i < node2->childs->size() && (*node2->childs)[ i ] )
                   {
                         
                        if( index < childs->size() && (*childs)[ index ]  )

                            (*childs)[ index ]->merge( (*node2->childs)[ i ] ) ; 


                    }       
                }  
                else // we have to insert the current item in this trie
                {                            
                    
                    // we insert this item not included in this trie and all these chilrens 
    				// , ie we insert the rest of the current itemset that is not 
    				// included in this trie 	

                    if(  node2->childs && i < node2->childs->size() && (*node2->childs)[ i ] )
                    {                         
                        addItVect( node2->offset + i, (*node2->childs)[ i ] ) ;
                     
                        (*node2->childs)[ i ]->id = node2->offset + i ;
                        (*node2->childs)[ i ]->parent = this ;
                           
                        // to don't delete the childs when deleting node2
                        
                        (*node2->childs)[ i ] = 0 ;
                    }   
                    else
                        addItVect( node2->offset + i, 0 ) ;

                    // copy the support
                    
                    (*cnts)[ node2->offset + i - offset ] = (*node2->cnts)[ i ] ;
                    
                }      	        	   
        	}   
    	    i++;
    	    
        }  
        
        node2->deleteChildren() ;
    }
    else
    {

       offset = node2->offset ;     
       cnts = node2->cnts ;
       node2->cnts = 0 ;
       childs = node2->childs ;
       node2->childs = 0 ;               

    }    
        
}

// ----------------------------------------------------------------------------------------------

/**
*  method that return the complementary of this set of itemset (res)
*  since we have all the items ordered and with the internal
*  id begining at 1 to the number of items in the database
*  we only have to use the total number of items to find the
*  complementary 
*/
template<class SetsType,class Measure>
Node< SetsType, Measure > *  Node< SetsType, Measure >::complem( vectUI & listElem, int & maxSize   ) 
{
    Node< SetsType, Measure > * res ;

	res = new Node< SetsType, Measure >( -1 ) ;
	
	vectUI * vect ;

	vect = new vectUI() ;

	if( cnts != 0 )
		complRec( listElem , vect, res,  maxSize ) ;

	else // we complement the empty set	  
	{
	    res->insert( &listElem );
	    maxSize = (int ) ( listElem.size() );
	}    

     
	delete vect ;
	
	return res ;

}

template<class SetsType,class Measure> 
void Node< SetsType, Measure >::complRec(  vectUI & listElem, vectUI * vect, Node< SetsType, Measure > * res, int & maxSize  ) 
{
    vectUI * comp ;
        
	for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + offset ) ;
    			  
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
       		    (*childs)[ i ]->complRec( listElem, vect, res, maxSize ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
           		 // so we complement it 
        	{
        		
        		comp = complem( vect, listElem ) ;
        		
        		if( maxSize < comp->size() )
        		          maxSize = (int) (comp->size() ) ;
        
        		if( comp->empty() == false )
        			res->insert( comp ) ;
      
        		delete comp ;
                 		    
       		}    
                                
   			vect->pop_back() ;
           }				
	}    
	
}

// ----------------------------------------------------------------------------------------------

/*
*  method that proces the complement of 
*  an itemset in whom the items are
*  between 1 and nb
*/
template<class SetsType,class Measure> 
vectUI * Node< SetsType, Measure >::complem( vectUI * vect, vectUI & listElem ) 
{
    int sizeTotElem =  listElem.size() ;
    int sizeToComplem = vect->size() ; 
    int cpteur  ; 
	vectUI * comp ;

	comp = new vectUI() ;
	comp->reserve( sizeTotElem - vect->size() ) ;

 
	// we complement the items who are before the first item of
	// the current itemset
		
	for( cpteur =  0 ; listElem[cpteur] < (*vect)[0]; ++cpteur ) // vect have to be included in listElem
			comp->push_back( listElem[cpteur]  ) ;	

    ++cpteur;

	// we add the complementary items between the first and the last
	// item of the current itemset

	for( int i = 1; i < sizeToComplem ; i++ )
	{
         while( (*vect)[ i ] >  listElem[cpteur] )
         {
         	comp->push_back( listElem[cpteur] ) ;
         	++cpteur;
         }  
         ++cpteur;        
	}

	// we complement the items who are after the last item of
	// the current itemset
	// <= since the internal id begin at 1
	
    while( cpteur < sizeTotElem ) 
    {	
		comp->push_back(  listElem[cpteur] ) ;	
        ++cpteur;  
    }

	return comp ;

}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that generate into this set 
*  the subsets of size "size" of an itemsets of size k
*/
template<class SetsType,class Measure>
int Node< SetsType, Measure >::genSubsets(  vectUI * itemset, int spos , int depth ) 
{
    Node< SetsType, Measure > * tmpCh ;
    int i ;
    
    int nb = 0 ;

	bool insert; // boolean used to count the sub itemset generated

	int loper = spos;
	spos = depth+1;
	

	while( --spos >= loper )
	{
	    
	    insert = false;
	     
		// we search in the set of itemset
		// the current item of a sub itemset of iset

		i = (*itemset)[ spos ] - offset;

		// we insert the curent item 
		// as a component of a subsets

		if( !cnts || i < 0 || i >= cnts->size() || ! existVal( i ) )
		{    
            addItVect( (*itemset)[ spos ] );	
            
            insert = true;	    
		}

		if( depth < itemset->size() -1 ) // creation of the child node
        {          
            i = (*itemset)[ spos ] - offset;
   		    
    	    if( !childs || i >= childs->size() || !(*childs)[ i ] )
            {
       	    	if( mem > 0 )
                    tmpCh = new Node< SetsType, Measure >( (*itemset)[ spos ], globalOffset,0,0,this); //, new vectUI(mem), 0 ) ;
                else                
                    tmpCh = new Node< SetsType, Measure >( (*itemset)[ spos ], (*itemset)[ spos+1 ],0,0,this);// , new vectUI( 1 ), 0 ) ;
 
                addItVect( (*itemset)[ spos ], tmpCh  );
   
                i = (*itemset)[ spos ] - offset;                                  
            }    

            nb+=(*childs)[i]->genSubsets( itemset,  spos+1, depth+1);			
       }
       else if ( insert )
					nb++ ;	// to have the total number of elements in the global trie

			
	}

	return nb;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method that return a set of the subsets of size
*  "size" of this set of itemsets 
*/
template<class SetsType,class Measure>
Node< SetsType, Measure > * Node< SetsType, Measure >::genSubsets( int size ) 
{
	Node< SetsType, Measure > * subset = 0 ;
	
	vectUI * itemset = new vectUI() ;
 
	if( cnts != 0 )
	{
		subset = new Node< SetsType, Measure >( 0 ) ;

		genSubsets( size, itemset, subset ) ;	
	}		
	
	delete itemset ;

	return subset ;

}

template<class SetsType,class Measure>
void Node< SetsType, Measure >::genSubsets( int size, vectUI * itemset, Node< SetsType, Measure > * subset ) 
{
          
	for( int i = 0; i < cnts->size(); i++)
	{
		if( existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			itemset->push_back( i + offset ) ;
    			  
   			// we continue searching with the next items
   
       		if( childs && i < childs->size() && (*childs)[ i ] )
            {	
       		    (*childs)[ i ]->genSubsets( size, itemset, subset ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
               	 // so we generate his subset
        	{        
        		subset->genSubsets( itemset, size ) ;		        
        	}
                                
   			itemset->pop_back() ;
           }				
	}       		
}


#endif
