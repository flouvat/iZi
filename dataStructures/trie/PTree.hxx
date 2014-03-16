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
 
#ifndef PTREE_HXX
#define PTREE_HXX

#include "Node.hxx"
#include "RecodeToInt.hxx"
#include "Boolean.hpp"

using namespace std;


//! Template class representing a trie  data structure of integer.
/**   
   This class store sets of integers in a trie. 
   The integers are stored in a "bool vector" data strucuture in the nodes.
   If ith value of the vector is not empty, it means that the element i is in the node

   The template parameter SetsType represents the type of the elements inserted in the trie.   
   The template parameter represents the type of the "bool vector" used to represents the presence or not of an element.
   For example bool to only store the presence or not of the element, int to store an integer measure (such as the support)...
*/
template< class SetsType, class T=Boolean > 
class PTree
{      
                    
      protected:
              
                //! Number of sets of integers in the trie.
                int    nbsets ;                 

                //! Height of the trie.
                int    height;       
    
                //! Root node of the trie.
                Node< SetsType, T  > * head;   
                               
   
                
      public:
            //! recode functor used to recode all the itemsets inserted in int and to keep the original order of the itemsets.
            static  RecodeToInt<SetsType> recode ;
          
            
            //! Used to have the same syntax tha the STL 
            typedef ItPTree<SetsType,T> iterator ;                          
             
            //! Constructor. 
            PTree( Node< SetsType, T > * inHead = 0, int inSize=0, int inHeight = 0 ) 
                     : head( inHead ), nbsets( inSize ), height( inHeight ) {}
                     
            //! Copy constructor.         
        	PTree( const PTree &t ) : head( t.head ), nbsets( t.nbsets ), height( t.height) {}
        	
        	//! Destructor.
        	~PTree()
             {      if( head){ head->deleteChildren() ; delete head;  head = 0; } }
        
             
            //! Insert a set of element in the trie.
            /**
                This function corresponds to the classic push_back( ) function od STL container.
                The template parameter represents a container of integers.
                \param setElement the set of element to insert in the trie.
                \param nb the number of times the set is inserted
                \return true if a new set has been inserted (false if it was already in the trie)
            */             
            template< class Container >
            void push_back(  Container& setElement, T nb = T(1)  )
            { 
                if( !head ) head = new Node< SetsType, T >(-1) ;

                int length = head->push_back( recode( setElement ) , nb );

                if ( length ) nbsets++ ;  
                if( height < length ) height = length ;
                                
            }
            
            //! Insert a set of element in the trie.
            /**
                The template parameter represents the input iterators on the integers to insert int the trie.
                \param first iterator on the first element 
                \param last iterator on the element after the last 
                \param nb the number of times the set is inserted
                \return true if a new set has been inserted (false if it was already in the trie)                
            */             
            template< class InputIterator>
            void push_back( InputIterator  first, InputIterator last, T nb = T(1)  )
            {       
                 if( !head ) head = new Node< SetsType, T >(-1) ;
                 int length = head->push_back( recode( first,last)  , nb );                
                 if ( length ) nbsets++ ;  
                 if( height < length ) height = length ;

                 
            }
            
            //! Erase a set in the trie.
            /**
                Delete a branch of the trie.
                \param position erase the nodes corresponding to the given iterator (which is on a leaf node).
            */
            void erase( iterator & position )
            {

                int index = position.ptr->deleteItVect( position.internalId - position.ptr->offset ) ;//the next index
             
                while( position.ptr->cnts == 0 ) // if there is no more elements in the node delete the node
                {                      
                    Node< SetsType, T > * parent = position.ptr->parent;
                    int indexprnt = position.ptr->id - parent->offset; 
                    
                    parent->deleteItVect( indexprnt ) ; // delete the parent node                    
                    
                    position.ptr = parent ;
                    position.internalId = indexprnt + parent ->offset ;
                    --(position.height) ;   
                }

            }
            
            //! Insert a set of element to the node pointed by an iterator.
            /**
                The template parameter represents a container of integers.
                \param position iterator on the node where the set must be inserted(the iterator is not mover during the operation) .
                \param setElement the set of element (internal id ) to insert in the trie.
                \param nb the number of times the set is inserted
            */             
            template<  class Container >
            void addToNode( iterator & position, Container& setElement, T nb = T(1)  )
            {                 
                 position->insertChildNode( position.index+position->getOffset(), recode( setElement ) , nb );                 
                 
                 int length = position.length + setElement.size() ;

                 if( height < length ) height = length ;
            }
            
            //! Insert a set of element to the node pointed by an iterator.
            /**
                The template parameter represents a container of integers.
                \param position iterator on the node where the set must be inserted (the iterator is not mover during the operation).
                \param first iterator on the first element
                \param last iterator on the element after the last
                \param nb the number of times the set is inserted
            */             
            template< class InputIterator>
            void addToNode( iterator & position, InputIterator first, InputIterator last, T nb = T(1) )
            {                  
                 vector< int > tmpset = recode( first, last );

                 position.ptr->insertChildNode( position.internalId, tmpset  , nb );                 

                 int length = position.length() + tmpset.size() ;

                 if( height < length ) height = length ;

            }
            
            //! Erase the node under the iterator 
            /**
                \param it erase the node at position it.
                \return iterator on the next node (ie, the next child node wrt lex order if exits, else the parent node ) .
            */    
            iterator eraseNode( iterator & it )
            {                     
                int index = it.ptr->deleteItVect( it.internalId - it.ptr->offset ) ;//the next index
        
                if( it.ptr->cnts == 0 ) // if there is no more elements in the node delete the node
                {   

                    Node< SetsType, T > * parent = it.ptr->parent;
                    if( parent )
                    {
                        int indexprnt = it.ptr->id - parent->offset; 
                        delete (*parent->childs)[ indexprnt ] ; //delete the node
                        (*parent->childs)[ indexprnt ]  = 0 ;   // delete the link in the parent node
                        it.internalId = indexprnt + parent ->offset ;                        
                    }
                    else
                        it.internalId = -1 ;
                    
                    it.ptr = parent ;

                    --(it.height) ;   
                }    
                else
                {
                    ++index ;
               
                    while( index < it.ptr->cnts->size() && (*it.ptr->cnts)[index] == T() ) // search the next child node
                           ++index ;                     

                     if( index == it.ptr->cnts->size() ) // no other child node, go to parent node
                     {
                         it.internalId = it.ptr->id   ;
                         it.ptr = it.ptr->parent ;
                         -- (it.height) ;                         
                     }                                
                }    
                        
                return it;
            }        
                                                         
            //! Return an iterator on the first leaf of the trie (exploring the trie from the left to the right). 
            ItPTree< SetsType, T > beginLeaf() { return ItPTree< SetsType, T >( head, &recode ); }                                                                                                      

            //! Return an iterator on the first leaf of the trie (exploring the trie from the right to the left). 
            ItPTree< SetsType, T > rbeginLeaf() 
            { 
                ItPTree< SetsType, T > it;
                it.ptr = head;
                
                it.internalId= head->cnts->size()+head->offset;
                it.recode = &recode ;
                it.height = 1 ;
                it.internalset=0;
                it.set=0; 
                    
                it=it.rnextLeaf();
                        
                return it; 
            }                                                                                                      
            
            //! Return an iterator on the root node of the trie. 
            ItPTree< SetsType, T > beginRoot() 
            {               
                ItPTree< SetsType, T > it;
                it.ptr = head;
                
                it.internalId= -1;
                it.recode = &recode ;
                it.height = 0 ;
                it.internalset=0;
                it.set=0;                     
                
                return it ;
            }                                                                                                      
                                                         
            //! Return an iterator on the first element stored. 
            ItPTree< SetsType, T > begin() {  return ItPTree< SetsType, T >( this );  };            
            //! Return an iterator on the element after the last element stored. 
            ItPTree< SetsType, T > end() { return ItPTree< SetsType, T >() ; } ;        
           
            //! Return the length of the longest branch of the trie
            int length() const { return height; }     
            
            //! Return the number of elements in the trie
            int size() const { return nbsets; }
            
            //! Sets the size of the trie to a given value.
            void setSize( int inSize ) { nbsets = inSize ; }

           
//            //! Find a set in the trie
//            /**
//                \retun iterator on the set to find
//            */
//            template< class Container >
//            ItPTree< SetsType, T > find( Container &  setElement )
//            { 
//            
//                // search the subset in the trie
//                 ItPTree< SetsType, T > it =  beginRoot();
//                 
//                 int i =0 ;
//                 it = it.childNode(setElement[i]) ;
//                 ++i;
////cout<<"while"<<endl;  
////cout<<" it.element() "<< it.element()<<endl;                 
//                 while( i < setElement.size() &&   it  != end()  )
//                 {
////cout<<"elem "<<  setElement[i] ;
//                    it = it.childNode(setElement[i]) ;
//        
////cout<<" it.element() "<< it.element()<<endl;                 
//                    ++i;
//                 }
//        
///**        cout<<"SUB  : ";
//        for_each( it->begin(), it->end(), printContainer() );
//        cout<<it.measure()<<endl;
//        getchar();   
//  */      
//                 return it ;    
//                
//            }        	                     
//             

        	//! Affectation operator.
        	PTree & operator = ( const PTree & inPTree ) 
        	{ if( head ) delete head ; head = inPTree.head ; nbsets = inPTree.nbsets ; height = inPTree.height; }
            
        	//! Operator < to process itemsets near the positive border
        	bool operator<(const PTree & inTrie ) const{return height < inTrie.height;}
        
        	//! Method that deletes recursively the sub tree of this node        
        	int deleteChildren()
               { int r = 0 ;
                 if( head ){ r = head->deleteChildren() ; delete head; head =0; }nbsets=0; return r; }
                         
      
        	//! Return the root node
        	Node< SetsType, T > * getHead() const{ return head; }

        	//! Initialize the root node
        	void setHead( Node< SetsType, T >* inHead ){ head = inHead ; }
        	
        	//! Initialize the height of the tree 
        	void setHeight( int inHeight ) { height = inHeight; }
        
        	// -------------------------------------------------
        	// method that increment the number of itemset
        	// -------------------------------------------------
        
        	int Increment( int inc = 1) {return nbsets+=inc;}
        
        	// -------------------------------------------------
        	// method that decrement the number of itemset
        	// -------------------------------------------------
        	
        	int Decrement( int dec = 1) {return nbsets-=dec;}
        
        	// ------------------------------------------------------
        	// method which insert an itemset in this set of itemset
        	// and return the node on the last element of the set
            // inserted
        	// ------------------------------------------------------
        
        	Node< SetsType, T > * insert( int * itemset, int level, T inSup = T(true) )
    	    { 
                Node< SetsType, T > * last = 0;         
                if( !head ) head = new Node< SetsType, T >(-1) ;
                if( last = head->insert( itemset, level, inSup) )
                { nbsets++ ;  if( height < level ) height = level  ;  }
                return last ;                    
            }
        
        	Node< SetsType, T > * insert( vectUI * itemset, T inSup = T(true) ) 
  	        { 
                Node< SetsType, T > * last = 0; 
                if( !head ) head = new Node< SetsType, T >(-1) ;
                if( last = head->insert( itemset, inSup ) )
                { nbsets++ ; if( height < itemset->size() ) height = itemset->size() ;  }
                return last ; 
            }
                
        	Node< SetsType, T > * insert( list< int > * itemset, T inSup = T(true) )
  	        { 
                Node< SetsType, T > * last = 0; 
                if( !head ) head = new Node< SetsType, T >(-1) ;
                if( last = head->insert( itemset, inSup ) )
                { nbsets++ ;  if( height < itemset->size() ) height = itemset->size() ;  }
                return last;
            }
        	    
        	// ---------------------------------------------------
        	// method which delete an itemset in the trie
            // ( all necessary mem reallocation 
        	// are did to keept the smallest representative trie)
        	// ---------------------------------------------------
        
        	void deleteIt( vectUI * itemset ) 
                { if( head ) if( head->deleteIt( itemset ) ) nbsets--;  }
        		
            // -----------------------------------------------------
        	// print to screen all the itemset stored in sub tree
        	// recode is a functor for mapping the name of the items
        	// ----------------------------------------------------
        
        	void print()
        	   { if( head ) head->print( &recode ) ; }
        	   
        	// ----------------------------------------------------
        	// print to screen all the supersets stored in sub tree
        	// (ie in the leaf node)
        	// recode is a functor for mapping the name of the items
           	// ----------------------------------------------------
        
        	void printSupersets()
        	   { if( head ) head->printSupersets( &recode ) ; }   
        
        	// ---------------------------------------------------
        	// read from a file all the itemset and stored them 
        	// into a prefix tree
        	// ---------------------------------------------------
        
        	bool readItemsets( const char * fileName, int * remap = 0 ) 
        	    { if( !head ) head = new Node< SetsType, T >() ; nbsets += head->readItemsets( fileName, remap ) ; return 1;  }
        	
        	// ---------------------------------------------------
        	// save into a file all the itemset stored in tree
        	// remap is a table for mapping the name of the items
        	// if remap is 0 it use the internal id
        	// ---------------------------------------------------
        
        	void saveItemsets( const char * fileName, int * remap = 0 )
        	   { if( head ) head->saveItemsets( fileName, remap ) ; }
        	
            void save( const char * fileName )
        	   { if( head ) head->save( fileName, &recode ) ; }   
        	
            // ---------------------------------------------------
        	// save into a file all the itemset stored in tree
        	// remap is a table for mapping the name of the items
        	// according the FIMI format of datasets 
        	// ---------------------------------------------------
        
        	void saveDataSet( const char * fileName, int * remap=0 ) 
        	   { if( head ) head->saveDataSet( fileName, remap ) ; }
        	
        	// -----------------------------------
        	// method searching if one itemset is 
            // included into itemset
        	// -----------------------------------
        
        	bool includedIn( int * itemset, int level ) 
        	   { if( head ) return head->includedIn( itemset, level ) ; else return false; }
        	
        	bool includedIn(  vectUI * itemset ) 
        	   { if( head ) return head->includedIn( itemset ) ; else return false; }
        
        	bool includedIn( list< int > * itemset)
               { if( head ) return head->includedIn( itemset, itemset->begin() ) ; else return false; }
        
        	// ----------------------------------------------
        	// method searching if itemset is include in one
        	// of the itemsets of this set of itemset
        	// ----------------------------------------------
        	
        	bool include( vectUI * itemset )
              { if( head ) return head->include( itemset ) ; else return false; }	
        
        	bool include( int * itemset, int level )
        	   { if( head ) return head->include( itemset, level ) ; else return false; }	
        
            // ----------------------------------------------------------------
        	// method which construct 
        	// Tr(i+1) = Si with Tr mininmal transversal
        	//				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
        	//				and H = { E1,..., En } an hypergraph
        	// and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
        	// Note: here Tr(i) is this, Ei+1 is itemset, level is the size
        	//       of Ei+1, bi is Bi  
        	//  this method is optimised for our problem since it don't add
        	//  to Bi the transversal of cardinality >= n-k-1 
        	//                        with n the number of items and
        	//                        k the level of the levelwise approach  
        	// currpos is the current pos in "itemset" for the subtree being
        	// tested
        	// ----------------------------------------------------------------
        
        	void initTrBiOpt( vectUI * itemset,  PTree * bi, int  maxTrans) ;
        
        	bool initTrBiOpt( Node< SetsType, T > * currNode, vectUI * itemset, PTree * bi, vectUI * vect, int & maxTrans,
                              int currpos=0 ) ;
                              	
        	// ----------------------------------------------------
        	// method generating the new itemsets for Tr(i+1) with 
        	// Tr(i), Bi, the new edge itemset
        	// this method is optimised for our problem since it 
        	// enable the separation of the minimal transversals
        	// that "generate" frequent already tested from the 
        	// others, without affecting the process of the 
        	// minimals transversals
        	// ----------------------------------------------------
        
        	PTree * newItTr( PTree * bi, vectUI * itemset, PTree * freqTr = 0 ) ;
        
        	void newItTr( Node< SetsType, T > * currNode, vectUI * itemset, 
                          list< int > * lst, PTree * res, PTree * freqTr = 0 ) ;
        
        	// -------------------------------------------------
        	// method merging this sets of itemset 
        	// and tr2 into this set 
        	// note: the set tr2 is destructed after the copy
        	// -------------------------------------------------
        
        	void merge( PTree * tr2 ) 
        	   { if( tr2 && tr2->head ){ if( head ) head->merge( tr2->head ) ; 
                            else{ head = tr2->head ; tr2->head = 0 ; }
                            nbsets+= tr2->nbsets ;
                            tr2->nbsets = 0 ;
                           }
              }
        
        	// -------------------------------------------------------------
        	// method which calcul the minimal transversal with i+1 edges
        	// from the minimal transversal with i edges ( this set ) 
        	// and a new edge ( Demetrovics et Thi approach )
        	//  this method is optimised for our problem since it delete 
        	//  all the transversal of cardinality >= n-k 
        	//                        with n the number of items and
        	//                        k the actual level of the
        	//							levelwise approach  
        	// it also enable the separation of the minimal transversals
        	// that "generate" frequent already tested from the 
        	// others, without affecting the process of the 
        	// minimals transversals
        	// -------------------------------------------------------------
        
        	void trMinIopt( vectUI * itemset, int  maxTrans, PTree * freqTr=0  );
        
        	// -------------------------------------------------------------
        	// method which calcul the minimal transversal with the edges
        	// stored in the set in parameter
        	// from the minimal transversal with i edges ( this set ) 
        	// and a new edge ( Demetrovics et Thi approach )
        	//  this method is optimised for our problem since it delete 
        	//  all the transversal of cardinality >= n-k 
        	//                        with n the number of items and
        	//                        k the actual level of the
        	//							levelwise approach  
        	// it also enable the separation of the minimal transversals
        	// that "generate" frequent already tested from the 
        	// others, without affecting the process of the 
        	// minimals transversals
        	// -------------------------------------------------------------
        
        	void trMinIopt( PTree * tr, int  maxTrans, PTree * freqTr=0  ) ;
        	

        	
        	void trMinIopt(  Node< SetsType, T > * currNode, int & maxTrans, PTree * freqTr, vectUI * vect  );
        	
        	template< class IteratorsContainer>
        	void trMinIopt( IteratorsContainer & listEdge, int  maxTrans, PTree * freqTr=0  )
        	{
           
                 for( typename IteratorsContainer::iterator it = listEdge.begin(); it != listEdge.end() ; ++it )
                 {
                      trMinIopt( it->getInternalset()   , maxTrans, freqTr ) ;
                 }
                 
            }
            
            //! Method which calcul the minimal transversal of this set of set.
        	/**
               This method is optimised for our problem since it delete 
               all the transversal of cardinality <= maxTrans and 
               do not conisder edge of size >= minEdge
        	*/       
        	PTree * trMinOpt( int  minEdge, int maxTrans );
        	
        	void trMinOpt( PTree< SetsType, T > * res, int  minEdge, int  maxTrans );
        	
        	void trMinOpt( Node< SetsType, T > * currNode,  PTree * res, int & minEdge, int & maxTrans, vectUI * vect ) ;        
        
        	// -----------------------------------------------------------------
        	// method that construct the complementary of this set of itemset
        	// ( res ) since we have all the items ordered and with the internal
        	// id begining at 1 to the number of items in the database
        	// we only have to use the total number of items to find the
        	// complementary
        	// -----------------------------------------------------------------
        
            template< class Container >
        	PTree * complem( Container & listElem ) 
        	    { 
                    PTree * res = 0;         	    
                    if( head )
                    {
                         res = new PTree(0, nbsets) ; 
                         res->height = 0;
                         res->head = head->complem( recode( listElem), res->height  ) ;  
                         
                    }
                    return res ;
                }
        
        	// ------------------------------------------
        	// method that generate into this set 
        	// the subsets of size "size" of an itemsets 
        	// Note: items must be already alocated
        	// -------------------------------------------
        
        	void genSubsets(  vectUI * itemset, int inSize )
        	    { 
                 if( ! head ) head = new Node< SetsType, T >() ;
                  nbsets += head->genSubsets( itemset, inSize ) ;
                  height = inSize ;
                }    
        
        	// ------------------------------------------------
        	// method that return a set of the subsets of size
        	// "nbsets" of this set of itemsets 
        	// ------------------------------------------------
        
        	PTree * genSubsets( int inSize ) 
        	    { PTree * res = 0; 
                  if( head ){ res = new PTree() ; res->head = head->genSubsets( inSize ) ;  res->height = inSize ; }
                  return res ;
                }
                
           
                
         friend class ItPTree< SetsType, T >;       

};

template< class SetsType, class T> RecodeToInt<SetsType> PTree< SetsType, T >::recode = RecodeToInt<SetsType>() ;

// ---------------------------------------------------------------------------------------------- 

/**
*  method which construct 
*  Tr(i+1) = Si with Tr mininmal transversal
*  				and Si = { X is in Tr(i) | X intersect Ei+1 != 0 }
*  				and H = { E1,..., En } an hypergraph
*  and Bi with Bi = { X is in Tr(i) | X intersect Ei+1 == 0 }
*  Note: here Tr(i) is the trie, Ei+1 is itemset, level is the size
*        of Ei+1, bi is Bi  
*  this method is optimised for our problem since it don't add
*  to Bi the transversal of cardinality >= n-k-1 
*                        with n the number of items and
*                        k the actual level of the levelwise approach  
* currpos is the current pos in "itemset" for the subtree being
* tested 
*/
template< class SetsType, class T>
void PTree< SetsType, T >::initTrBiOpt( vectUI * itemset,  PTree< SetsType, T > * bi, int maxTrans  )
{
	vectUI * vect ;

	vect = new vectUI ;

	if( head )
    {
        initTrBiOpt( head, itemset, bi, vect, maxTrans );         
    }    
	
	delete vect ;

}

template< class SetsType, class T>
bool PTree< SetsType, T >::initTrBiOpt( Node< SetsType, T >* currNode, vectUI * itemset, PTree< SetsType, T > * bi, vectUI * vect, int  & maxTrans, int currpos )
{

    int i ;
	
	bool intersect = false ;
    
    i = 0 ;
	
      	while( currNode->getCnts() && i < currNode->getCnts()->size()  )
       	{
    
       		if( currNode->existVal( i ) )
       		{    		    
       		    while( ( currpos < itemset->size() ) && ( i+  currNode->getOffset() > (*itemset)[ currpos ] ) )
       		              currpos++ ;
        		     		       			
           		// we save the curent item of the itemset being processed
            
           		vect->push_back( i +  currNode->getOffset() ) ;
                	      			
           		// we continue searching with the next items
    
               	if( currpos >= itemset->size() || i+  currNode->getOffset() != (*itemset)[ currpos ] )
                {
                     if( currNode->getChilds() !=0 && i < currNode->getChilds()->size() && (*currNode->getChilds())[ i ] )
                     {	
                        intersect = initTrBiOpt( (*currNode->getChilds())[ i ], itemset, bi, vect, maxTrans ,currpos ) ;
   
                        // if there is no intersection 
                   		// we delete the motif from Tr(i+1)

                 		if( intersect == false  &&  
                             (  (*currNode->getChilds())[ i ]->getCnts()  == 0 ) )
                   		{            		
                          	                			    
                  			(*currNode->getChilds())[ i ]->deleteChildren() ;
                    				
                   			delete (*currNode->getChilds())[ i ] ;
                   			(*currNode->getChilds())[ i ] = 0 ;

                   			if( mem > 0 )
                   			      currNode->setCnt( i, 0 ) ;
                            else
                                  i = currNode->deleteItVect( i )  ;  

                   		}                		
        
               	    }    
                   	else // we have check all the items of the current itemset strored in vect
                   	     // and there is no intersection. so we add this itemset to Bi
                   	{
                       
                  		if(  vect->size() < maxTrans ) 
           					bi->insert( vect, currNode->getCnt( i ) ) ;   
                         
                        nbsets--;             
                        // return the new index for i            
                       if( mem > 0 )
                   			currNode->setCnt( i, 0 );
                       else
                            i = currNode->deleteItVect( i )  ;
                      
        
                   	}    
                }   	
                else
                {
                    intersect = true ;
                }                
               	vect->pop_back() ;
                                                      			       						
            }
        	i++ ;
    
        }
           
	return intersect ;
}

// ---------------------------------------------------------------------------------------------- 

/**
*  method generating the new itemsets for Tr(i+1) with 
*  Tr(i), Bi, the new edge itemset
*  this method is optimised for our problem since it 
*  enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/
template< class SetsType, class T>
PTree< SetsType, T > * PTree< SetsType, T >::newItTr( PTree< SetsType, T > * bi, vectUI * itemset, PTree< SetsType, T > * freqTr ) 
{
	PTree< SetsType, T > * res ;		

	list< int > * lst ;

	res = new PTree< SetsType, T >() ;

	lst = new list< int >() ;

	if( bi->head )
	{	    
		newItTr( bi->head, itemset, lst, res, freqTr ) ;	
	}	
    else
    {
        for( vectUI::iterator it =itemset->begin(); it != itemset->end(); ++it )
             res->insert( &(*it) , 1 ) ;
        
    }
    		   
	delete lst ;
		
	return res ;	

}

template< class SetsType, class T>
void PTree< SetsType, T >::newItTr( Node< SetsType, T > * currNode, vectUI * itemset, 
                    list< int > * lst, PTree< SetsType, T > * res, PTree< SetsType, T > * freqTr  ) 
{	

	list< int >::iterator tmpl ;

    int i= 0 ;
    while( currNode->getCnts() && i < currNode->getCnts()->size() )
	{
  
		if( currNode->existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			lst->push_back( i + currNode->getOffset() ) ;

   			// we continue searching with the next items
   
       		if( currNode->getChilds() && i < currNode->getChilds()->size() && (* currNode->getChilds() )[ i ] )
            {	
       		    newItTr( (*currNode->getChilds() )[ i ], itemset, lst, res, freqTr ) ;
   		    
	        }    
       		else // we have check all the items of the current itemset strored in vect
       		{
      		    for( int j = 0; j < itemset->size() ; j++ )
        		{
        			// we generate a new candidate which might be in the minimal transversal
        
        			lst->push_back( (*itemset)[ j ] ) ;

        			// we save the place of the item that
        			// we have just inserted
        
        			tmpl = lst->end() ;
        			tmpl-- ;
        
        			lst->sort() ;
                      
        			// we test that all the itemsets of Si are not inluded into our candidate

        			if ( ( includedIn( lst ) == false )
        				 && ( ( freqTr == 0 ) ||( freqTr->includedIn( lst ) == false ) ) 
                        )        		
             
                        res->insert( lst ) ;     
               
        			lst->erase( tmpl ) ;    
        		}
     		}                                    
   			lst->pop_back() ;
            
        }
        i++;
	}
	     currNode->deleteChildren();
	
}
	
// ---------------------------------------------------------------------------------------------- 

/**
*  method which calcul the minimal transversal with i+1 edges
*  from the minimal transversal with i edges ( this set ) 
*  and a new edge ( Demetrovics et Thi approach )
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*  it also enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/
template< class SetsType, class T>
void PTree< SetsType, T >::trMinIopt( vectUI * itemset, int maxTrans, PTree< SetsType, T > * freqTr )
{
	PTree< SetsType, T > * bi ;

	PTree< SetsType, T > * res ;
	
	bi = new PTree< SetsType, T >() ;

	// construction of Si and Bi
	// Si is stored into this set
	// in fact we search all the itemset of
	// this set that don't intersect itemset,
	// we remove them from tr and 
	// we insert them into bi

	initTrBiOpt( itemset, bi, maxTrans) ;

	// construction of the new minimal transversal
	// itemsets, construct from all the itemset
	// of this set that don't insersect itemset and the items
	// of itemset,
	// this set of itemset is returned by the method

	res = newItTr( bi, itemset,  freqTr ) ;

	delete bi;
	
	// add the news minimal transversal itemsets
	// to tr and delete the set res
 
    if( height < res->height )
        height = res->height ;
       

	merge( res ) ;


	delete res;
}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal with the edges
*  stored in the set in parameter
*  from the minimal transversal with i edges ( this set ) 
*  and a new edge ( Demetrovics et Thi approach )
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*  it also enable the separation of the minimal transversals
*  that "generate" frequent already tested ("freqTr") from the 
*  others, without affecting the process of the 
*  minimals transversals
*/
template< class SetsType, class T>
void PTree< SetsType, T >::trMinIopt( PTree< SetsType, T > * tr, int maxTrans, PTree< SetsType, T > * freqTr  ) 
{
	vectUI * vect ;

	vect = new vectUI ;

	if(  tr->head ) 
		trMinIopt( tr->head, maxTrans, freqTr, vect ) ;
	
	delete vect ;
}

template< class SetsType, class T>
void PTree< SetsType, T >::trMinIopt(  Node< SetsType, T > * currNode, int & maxTrans, PTree< SetsType, T > * freqTr, vectUI * vect  ) 
{   
	for( int i = 0; i < currNode->getCnts() ->size(); i++)
	{
		if( currNode->existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + currNode->getOffset() ) ;
    			
   			// we continue searching with the next items
   
       		if( currNode->getChilds() && i < currNode->getChilds()->size() 
                           && (* currNode->getChilds() )[ i ] )
            {	
       		    trMinIopt( (* currNode->getChilds() )[ i ], maxTrans, freqTr, vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
                 // so we insert the new edge in the hypergraph
        	{                 
        		trMinIopt( vect, maxTrans, freqTr ) ;	
           	}    
                                
   		vect->pop_back() ;
        }
				
	}

}

// ----------------------------------------------------------------------------------------------

/**
*  method which calcul the minimal transversal of this set of
*  itemset
*  this method is optimised for our problem since it delete 
*  all the transversal of cardinality >= n-k 
*                        with n the number of items and
*                             k the actual level of the
*								levelwise approach  
*/
template< class SetsType, class T>
PTree< SetsType, T > * PTree< SetsType, T >::trMinOpt( int  minEdge, int  maxTrans )
{
	PTree< SetsType, T > * res ;

	vectUI * vect ;

	res = new PTree< SetsType, T >() ;
	
	vect = new vectUI() ;

	if( head != 0 )
		trMinOpt( head, res,minEdge, maxTrans, vect ) ;
		
	delete vect ;

	return res ;

}

template< class SetsType, class T>
void PTree< SetsType, T >::trMinOpt( PTree< SetsType, T > * res, int  minEdge, int  maxTrans )
{

	vectUI * vect ;

	vect = new vectUI() ;

	if( head != 0 && res )
		trMinOpt( head, res,minEdge, maxTrans, vect ) ;
		
	delete vect ;


}

template< class SetsType, class T>
void PTree< SetsType, T >::trMinOpt( Node< SetsType, T > * currNode,  PTree< SetsType, T > * res, int & minEdge, int & maxTrans, vectUI * vect ) 
{        
	for( int i = 0; i < currNode->getCnts()->size(); i++)
	{
		if( currNode->existVal( i ) )
		{
   			// we save the curent item of the itemset being processed

   			vect->push_back( i + currNode->getOffset() ) ;
    			   
   			// we continue searching with the next items
   
       		if( currNode->getChilds() && i < currNode->getChilds()->size() && (* currNode->getChilds() )[ i ] )
            {	
       		    trMinOpt( (* currNode->getChilds() )[ i ], res, minEdge, maxTrans, vect ) ;
	        }    
       		else // we have check all the items of the current itemset strored in vect
                 // so we insert the new edge in the hypergraph
        	{  
                if( vect->size() >= minEdge ) 
                {                          
                    if( ! res->head )
                	{  	// if there is no itemset in bi
                		// the new min transversals will simply be all the items of itemset
              
                		res->head = new Node< SetsType, T >(0, (*vect)[0],0,0 ) ;
                		
                 		res->head->setCnts( new vector< T >( (*vect)[ vect->size()-1 ] - (*vect)[0] +1 )  ) ;
                
                		for( int i= 0; i < vect->size(); i++ )            		
                		    res->head->setCnt( (*vect)[i] - res->head->getOffset() , 1 )	;
                      		
                        res->nbsets = vect->size() ;	    		    
                	}     
                    else                                            	    
                        res->trMinIopt( vect,maxTrans,0 ) ;	
                }
           	}                                    
   		vect->pop_back() ;
        }				
	}	
}
// ----------------------------------------------------------------------------------------------


#endif
