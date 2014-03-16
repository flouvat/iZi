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
 
#ifndef INITFREQUENT_HXX
#define INITFREQUENT_HXX

#include <utility>

#include "RecodeToInt.hxx"

    
//! Functor used to initialize the frequent items.
/**
   This functor is used to search the items in the database with their support.
   It is passed in parameter of the algorithms and execute int the algorithm.
   
   The predicate is used  in this class to store the items with their support.
   
   The template parameter SetsType is the type of the items.
   The template parameter BinaryDB is the type of the transactional database.
   The template parameter Predicate is the type of the predicate used to select interesting items. 
*/
template< class SetsType, class BinaryDB, class Predicate >
class InitFrequent
{
        typedef  pair<SetsType, int> ItemSupp ;
        typedef  list<ItemSupp > listItemSupp ;
        typedef typename   listItemSupp::iterator ItListItemSupp ;
        typedef typename   listItemSupp::reverse_iterator revItListItemSupp ;
             
        //! Functor extracting all the items of the container of the db and updating their support.    
        class searchFIDB 
        {
                //! items store the items with their support
                listItemSupp * items ;
                
            public:
                                                    
                searchFIDB( listItemSupp * initems ){ items =  initems; }            
                
                template< class T1 >     
                void operator() (T1 x) { for_each( x.begin(), x.end(), searchFITrans( items )  ); }
        };   
         
        //! Functor extracting all the items of a transaction and updating their support.
        class searchFITrans 
        {
                //! items store the items with their support
                listItemSupp * items ;
                
            public:
                                                    
                searchFITrans( listItemSupp * initems ){ items = initems; }            
                
                void operator() (SetsType x) 
                {            
                    ItListItemSupp it = find_if( items->begin(), items->end(), eqItemSupp(x) )  ;
                    
                    if( it == items->end() ) // the items was not dicovered
                         items->push_back( make_pair( x, 1 ) );
                    else // update the support
                         it->second++;        
                }
        };               
        
        //! Functor used to find an item in the list of items/support.
        class eqItemSupp 
        {         
                  //! item to find      
                  SetsType item ;            
              public:                   
                  eqItemSupp(  SetsType & it1  ) { item = it1; }     
                  
                  bool operator ()( ItemSupp & it2) { return item == it2.first; }
        };
        
        //! Functor used to order the items wrt their support (increasing order).
        struct lessItemSupp 
        {         
                  bool operator ()( const ItemSupp & it1,  const ItemSupp & it2) { return it1.second < it2.second; }
        };    
        
    protected:
       
        //! The database.
        BinaryDB * db ;
        
        //! The predicate.                      
        Predicate * pred; 
        
       
    public:
           
        //! List of the items
        deque<SetsType> listElements;
           
        //! Constructor
        InitFrequent(   BinaryDB & indb, Predicate & inpred ){ db = & indb; pred = & inpred; }        
        
        //! Operator excuting this initialization function in the algorithm.
        void operator() ();
         
};


//! Operator excuting this initialization function in the algorithm.
template< class SetsType, class BinaryDB, class Predicate  > 
void InitFrequent<SetsType, BinaryDB, Predicate>::operator() ()
{
     
    int nbElements = 0;
    clock_t start = clock();
    
    // extract items and their support
    for_each( db->begin(), db->end(), searchFIDB( & (pred->listItemSupport) ) ) ; 

    // reorder items (increasing order of support)
    pred->listItemSupport.sort( lessItemSupp() ); 
                
    // recode items and insert in the candidates data structure in the specified order
    ItListItemSupp it= pred->listItemSupport.begin();
    while( it != pred->listItemSupport.end() )
    {
        listElements.push_back( it->first);
        ++it ;
    }  

}    


#endif

