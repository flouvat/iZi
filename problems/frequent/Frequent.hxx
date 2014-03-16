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
 
#ifndef FREQUENT_HXX
#define FREQUENT_HXX

#include "Predicate.hxx"
#include "RecodeToInt.hxx"

#include "Node.hxx"
#include "Support.hpp"


//! Functor representing the predicate being frequent. 
/**
    This functor test if an itemset is frequent.
    This functor process the support before pruning.  
    We suppose that the data has the same internal encoding  than the candidates (ie, the first item met is recoded in "0", the second in "1", ...).
    
    The methods of this predicate are specific to tries data structure (PTree for the candiates and Tatree for the transactions).
    This functor cannot be used when using a transformation function (for exmaple to change the exploration of the search space).
    
    The template parameter Data is the type of the transactional database.
    The template parameter SetsType is the type of the items.    
*/
template< class Data, class SetsType>
class Frequent: public Predicate
{
      protected:
          
        typedef  pair<SetsType, int> ItemSupp ;
        typedef  list<ItemSupp > listItemSupp ;
        typedef typename   listItemSupp::iterator ItListItemSupp ;
        typedef typename   listItemSupp::reverse_iterator revItListItemSupp ;
        
        //! Functor used to find an item in the list of items/support.
        class eqItemSupp 
        {         
                  //! item to find      
                  SetsType item ;            
              public:                   
                  eqItemSupp(  SetsType & it1  ) { item = it1; }     
                  
                  bool operator ()( ItemSupp & it2) { return item == it2.first; }
        };
                
        //! Functor used to reorder a transaction            
        class order
        {      
                typedef typename map<SetsType,int>::iterator  mapDTintIt;  
               
                //! transaction recoded
                vector<int> * result;
                              
                //! Recode functor used to reorder the transactions.
                RecodeToInt<SetsType> * recode ;
                
                //! number of unfrequent items
                int nbnotfreq ;  
                    
            public:
                
                order( vector<int> * inres, RecodeToInt<SetsType> * inrec, int & innotnbfreq){ result = inres; recode = inrec; nbnotfreq = innotnbfreq ;}
                
                void operator()( SetsType & elem)
                {
                    mapDTintIt itmap ;
                    itmap = (recode->getIdTointernid())->find( elem ) ;
                    
                    if( itmap != (recode->getIdTointernid())->end() &&  itmap->second >= nbnotfreq ) 
                       result->push_back(  itmap->second ) ;  
                }                    
        };      
        
        //! Functor that reorders a transaction and insert it in the db.
        /**       
           The template parameter BinaryDB is the class representing the bianry database
        */
        class reorderTrans
        {
                //! database to recode.
                Data * dbr;  
                
                //! number of unfrequent items stored     
                /**
                   Used to filter unfrequent items from the transactions.
                   Since items are recoded wrt to increasing orders of support from 0 to number of items,
                   an items with an internal id < number of items - number of frequent items is necessarely unfrequent (if unfrequent items are stored).      
                */
                int nbnotfreq ;  
                
                 //! Recode functor used to reorder the transactions.
                RecodeToInt<SetsType> * recode ;
                                                    
            public:
                   
                //! Contructor 
                reorderTrans( Data * indb, int  nbfreq, RecodeToInt<SetsType> * inrecode  ) 
                { 
                     dbr = indb ;                 
                     recode = inrecode;
                                          
                     nbnotfreq = recode->size() - nbfreq  ; 
                     
                }
                
                
                //! Destructor.
                ~reorderTrans(){}                
                
                template< class Container >
                void operator() ( Container & trans )
                {
                     vector<int> tmptrans;
                     tmptrans.reserve( trans.size() );
                     
                     // recode the transaction wrt recode and delete the items not already recoded in recode
                     // ie recode the transactions without unfrequent items
                     for_each( trans.begin(), trans.end(), order( & tmptrans, recode, nbnotfreq ) );
        
                   
                     // reorder the transaction in decreasing order of support
                     sort( tmptrans.begin(), tmptrans.end()) ; 
                                
                     // recode th internal id with the origianl ones keeping the new order                               
                     //  this push_back on a binaryDB remove the trans for the tmp datastructure and insert the recoded trans in the final db
                     dbr->push_back( tmptrans ) ; 
                }     
        };  
        
                          

      
        //! Method used to count and save the support of a set of itemsets
        /**
            \param currNode curent node of the trie storing the itemsets
            \param itData iterator on the data
        */
        template< class Measure, class IteratorData >       
        int count( Node<int, Measure> * currNode, IteratorData  itData  ) ;
       
    protected:
    
        //! Internal encoding of the items    
        RecodeToInt<SetsType> recode ;
        
        //! The transactionnal database    
        Data * db; 
        
        //! The minimum support threshold (absolute)
        int minsup;
        
        
    public: 
            
        bool verbose ; // to print to screen 
            
        //! List of all the items with their support
        /**
            Used to avoid support counting for items since in the initialization functor the items support is processed.
            Set in the initialization functor.
        */    
        listItemSupp   listItemSupport ;     
                            
        //! Constructor
        /**
            \param indb the transactional database
            \param inMinsup the absolute minimum support threshold
        */
        Frequent( Data & indb, int inMinsup ){ db = & indb ; minsup = inMinsup; verbose = false; }
 
        //! Destructor
        ~Frequent(){}
                
        //! Operator that test if an itemet is frequent or not
        /**
            \param itemsetIt iterator (or pointer) on the itemset to test wrt the predicate
            \param mesCand value of the support of the itemset
        */
        template< class Iterator, class Measure >
        bool operator() ( Iterator   itemsetIt, Measure & mesCand  )
        { 
             if( itemsetIt->size() == 1 ) // use the items process in the initialization phase
             {
                 ItListItemSupp it = find_if( listItemSupport.begin(), listItemSupport.end(),  eqItemSupp( * itemsetIt->begin() ) );
                 
                 mesCand.supp = it->second ;  
            
                if( it->second < minsup  ) //erase unfrequent items
                 {
                     recode.getIdTointernid()->erase( it->first );  // used to recode the transaction  with only the frequent items                                    
                 }

             }
                          
             mesCand.presence=false; // to not count the support of this itemset in the next iteration 

             return mesCand >= minsup ;               
        }    
         
        //! Function used to count the support of the itemsets 
        /**
            Method used to count the support of itemsets.
            For each transaction of the db update the supports of all the itemsets in cand.
            \param cand container of words of the language.
            \param wordToset functor that eventually transforms itemsets
        */
        template< class Cand_DataStruct, class f >
        void preProcessing( Cand_DataStruct & cand , f  wordToSet  );
        
        //! Function used to do some post processing operations after testing the candidates generated.
        /**
            Method used to reconstruct the db after discovery of frequent items.
            Prune from the transactions all the not frequent items.
            \param cand container of words of the language.
            \param wordToset functor that eventually transforms itemsets
        */
        template< class Cand_DataStruct, class f>
        void postProcessing( Cand_DataStruct & cand, f  wordToSet );
        
};   

//! Function used to count the support of the itemsets 
/**
    Method used to count the support of itemsets.
    For each transaction of the db update the supports of all the itemsets in cand.
    \param cand container of words of the language.
    \param wordToset functor that eventually transforms itemsets
*/
template< class Data, class SetsType  > template< class Cand_DataStruct, class f >
void Frequent<Data,SetsType>::preProcessing( Cand_DataStruct & cand , f  wordToSet )
{
                                                       
    clock_t start ;    
    
    // copy all the items and their encoding
    recode = cand.recode ;
       
	if(  cand.getHead() &&  cand.length() > 1 )  // do not count support for items
	{
        
    	int used = 0 ;
        
        start = clock();

      	used += count( cand.getHead(), db->data().beginRoot()  ) ;
       	
       	if( verbose ) cout<<" Count ["<<(clock()-start)/CLOCKS_PER_SEC<<"s] ";
    
    }

}

//! Method used to count and save the support of a set of itemsets
/**
    \param currNode curent node of the trie storing the itemsets
    \param itData iterator on the data
*/    
template< class Data, class SetsType > template< class Measure, class IteratorData > 
int Frequent<Data,SetsType>::count(Node< int, Measure> * currNode, IteratorData  itData   )
{
    typedef Node<int,Measure> Node;
    IteratorData   it = itData ;

    itData.rbeginChildNode();
   
    if( currNode == 0 || itData == IteratorData() ) return 0;

    while( itData !=  IteratorData() )      // count the transactions recursively  
    {
        count( currNode, itData);
        itData.rnextChild() ;
    }
    
    int used=0 ;
    int size ;
    int i;
    int n;

    vector< Node *> * childs = currNode->getChilds(); 


    if( ! childs  || childs->empty() )
    {       // if this is a new node  
        n = currNode->getOffset();           // get the index offset    

        it.rbeginChildNode(); 

        while(it !=  IteratorData()  )
        {
            i = it.elementId() - n  ;   // traverse the items  
            
            if( i < 0 ) return used;        // if before first item, abort  
            
            if(  i < currNode->getCnts()->size() 
                  &&  currNode->existVal( i ) && currNode->getCnts()->at(i).presence )       // if inside the counter range  and the support has not already been updated
            {                 
                  currNode->getMeasure(i)=currNode->getMeasure(i)+Measure(1,it.measure() ); 
            }
            
            it.rnextChild() ;
        }
    }   // count the transaction  
    else 
    {   // if there are child nodes  
        
        Node * next ;
        
        n   = currNode->getOffset() ;

        it.rbeginChildNode();

        while( it !=  IteratorData() )
        {               
            i = it.elementId() -n;   // traverse the items  
      
            if (i < 0 ) return used ;      // if before first item, abort                               
            
            if ( ( i < childs->size() ) && ( next = (*childs)[i] ) )
            {
                                               
               count( next, it );
            }
            else              
            if(  i < currNode->getCnts()->size() 
                      &&  currNode->existVal( i ) && currNode->getCnts()->at(i).presence )       // if inside the counter range  and the support has not already been updated 
            {
                  currNode->getMeasure(i)=currNode->getMeasure(i)+Measure(1,it.measure() ); 
            }     
              
            it.rnextChild() ;  
        }                           // if the child exists,  
    }                               // count the transaction recursively  
    return used;
}

//! Function used to do some post processing operations after testing the candidates generated.
/**
    Method used to reconstruct the db after discovery of frequent items.
    Prune from the transactions all the not frequent items.
    \param cand container of words of the language.
    \param wordToset functor that eventually transforms itemsets
*/
template< class Data, class SetsType  > template< class Cand_DataStruct, class f>
void Frequent<Data,SetsType>::postProcessing( Cand_DataStruct & cand, f  wordToSet )
{

     if( cand.length() == 1 )
     {

            clock_t start = clock();            
                             
            // reorder the transactions of the data base (decreasing order of support)
            // each transaction is read,
            // recoded (same reconding than the one for the candidates) and re inserted in the data base
            reorderTrans  reorder( db, listItemSupport.size(), &recode ); 
            for_each( db->begin(), db->end(), reorder  ) ; 
            
            RecodeToInt<SetsType> * recodeData = new RecodeToInt<SetsType>() ;
            *recodeData = recode ;
            // set the recode function
            db->setRecode( recodeData );
             
            if( verbose ) 
            {                             
              cout<<"Reconstruct data ["<<(clock()-start)/CLOCKS_PER_SEC<<"s]"<<endl;   
            }

     }       

}



#endif 
