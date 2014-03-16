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
 
#ifndef Essential_HXX
#define Essential_HXX

#include <algorithm>

#include "Frequent.hxx"
#include "SupportDisj.hpp"
#include "TestSubsets.hxx"


//! Functor representing the predicate being frequent essential. 
/**
    This functor test if an itemset is frequent and essential.
    This functor process the support and the disjunction before pruning.  
    We suppose that the data has the same internal encoding  than the candidates (ie, the first item met is recoded in "0", the second in "1", ...).
    
    The methods of this predicate are specific to tries data structure (PTree for the candiates and Tatree for the transactions).
    For the moment, this functor can only be used when using a levelwise exploration of the search space (Apriori) since it checks for subsets already generated.
       
    The template parameter Data is the type of the transactional database.
    The template parameter SetsType is the type of the items.    
*/
template< class Data, class SetsType>
class Essential: public Frequent< Data, SetsType>
{
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
        
       
        typedef typename vector< int >::iterator ItVectInt;
             
                
        //! Method used to count the disjunction of a set of itemsets
        /**
            \param itbegin iterator on the first element of the itemset 
            \param itend iterator on the last element of the itemset 
            \parm itTat iterator on the root node on the tatree storing the database
            \return the number of transactions intersecting the itemset 
        */
        template< class Iterator >
        int countDisj( ItVectInt itbegin, ItVectInt itend, Iterator itTat  ) ;
        
       //! Functor used int the TestSubsets functor to test if an itesmet is essential 
       /**
           Test if two itemsets have the same disjunction values.
       */
       class equalDisj
       {
        public:
               
               template< class Iterator >
               bool operator()( Iterator it1 , Iterator it2 )
               {
                    return it1.measure().disj == it2.measure().disj ;
               }              
            
       };
        
    public: 
                                
        //! Constructor
        /**
            \param indb the transactional database
            \param inMinsup the absolute minimum support threshold
        */
        Essential( Data & indb, int inMinsup ):Frequent< Data, SetsType>(indb,inMinsup){}
 
        //! Destructor
        ~Essential(){}
                
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

                 ItListItemSupp it  = find_if( this->listItemSupport.begin(), this->listItemSupport.end(),  eqItemSupp( * (itemsetIt->begin()) ) );
                            
                 mesCand.supp = it->second ;  

                 if( it->second < this->minsup  ) //erase unfrequent items
                 {
//                    this->listElements->pop_front();   // since the items are stored in increasing order of support
                                                  // prune always the first element  
                                                  
                                  // prune always the first element  
                                                  
                     this->recode.getIdTointernid()->erase( it->first );  // used to recode the transaction  with only the frequent items                                    
                 }

                 mesCand.disj = it->second ;  

                 mesCand.presence=false;
                                  
                 return mesCand >= this->minsup;
             }
                        
             mesCand.presence=false; // to not count the support of this itemset in the next iteration 


             if(  mesCand >= this->minsup )
             {
                  mesCand.disj = countDisj( itemsetIt->begin(), itemsetIt->end(),  this->db->data().beginRoot() );
                  
                  TestSubsets<SetsType> isNotEssential;

                  return ! isNotEssential( itemsetIt, equalDisj() );                  
             }
              
             return false ;               
        }    
        
};   



//! Method used to count and save the support of a set of itemsets
/**
    \param currNode curent node of the trie storing the itemsets
    \param itData iterator on the data
*/    
template< class Data, class SetsType > template< class Iterator > 
int Essential<Data,SetsType>::countDisj( ItVectInt itbegin, ItVectInt itend, Iterator itTat )
{
    int disj = 0 ;

    itTat.beginChildNode() ;
    Iterator end ; //end iterator
    
    vector< int >::iterator it = itbegin ;
    vector< int >::iterator last = itend ;
    --last;
    
    int id; // internal id of the current element 
            // we use the internal id to easily know the order of the elements

    while( itTat != end && it != itend && itTat.elementId() <=  ( this->recode( *last) ) )
    {
           id = this->recode( *it) ; 

           if( itTat.elementId() == id )
           {
               disj += itTat.measure();
               ++it ;
              
               itTat.nextChild() ;    
           }
           else if( itTat.elementId() > id )
           {
               ++it;                     
           }
           else
           {         
               disj += countDisj( it, itend, itTat ) ;
               itTat.nextChild() ;
               
           }                      
    }       
    return disj;
}



#endif 
