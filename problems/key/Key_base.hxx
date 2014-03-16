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
 
#ifndef KEY_BASE_HXX
#define KEY_BASE_HXX

#include<set>
#include<utility>

#include "Id.hpp"
#include "Predicate.hxx"
#include "PTree.hxx"


//! Functor representing the predicate being not redundant. 
/*!
    This functor test if an itemset is a super key.  
    The method used to test if a candidate is a super key is the following one:        
        * the functor process the cardinality of the projection wrt the attributes of the candidate.
        * return false if the cardinality is not equal  to the number of tuples in the relation.    
    
    The template parameter Data is the type of the tablular data.
    The template parameter AttributeType is the type of the attributes.
*/
template< class Data, class AttributeType >
class Key_base: public Predicate 
{
    protected:
                    
      
        //! function used to count the projection of a candidates pointed by the iterator passed in parameter.  
        /*!
            \param itCand iterator on the candidate to test.
            \return the cardinality of the projection.
        */          
        template< class Iterator  >                  
        int countProjection( Iterator itCand ) ;
              
        //! Functor used to projet all the tuples wrt attributes studied 
        template< class Iterator>
        class ProcessTuples
        {            
                 //! Iterator on the candidate 
                 Iterator * itcand;      
                 
                 //! Store all the tuple sprojected wrt the candidate
                 PTree<int,bool> * projectTrie;
                 
                 vector<int> tupleProjected;
                 
                 RecodeToInt< AttributeType > * numAttrib ;
            
            public:
                 ProcessTuples( Iterator * initcand, PTree<int,bool> * inproject, RecodeToInt< AttributeType > * innumAttrib  ){ itcand=initcand; projectTrie = inproject; numAttrib=innumAttrib; }
                 
                 template< class ContainerTuple >
                 void operator() ( ContainerTuple &  tuple)
                 {
                      vector<int> * cand = & (*numAttrib)(**itcand);

                      tupleProjected.resize( 0 );
                      tupleProjected.reserve( cand->size() );                      

                      for_each(  cand->begin(), cand->end(), project<ContainerTuple>( &tuple, &tupleProjected) );

                      projectTrie->push_back( tupleProjected ) ;                          

                 }                                   
        };
       
        //! Functor used to projet a tuple wrt the set of attributes studied
        template< class ContainerTuple >
        class project
        {
                 ContainerTuple * tuple ;
                 
                 vector<int> * tupleProjected ;     
                                 
            public:                     
                 project( ContainerTuple * intuple, vector<int> * intupleProjected ){ tuple = intuple; tupleProjected = intupleProjected; }
                 
                 void operator() ( int  attrib )
                 {
                      tupleProjected->push_back( (*tuple)[ attrib ] ) ; 
                 }                                            
        };       
       
        

    protected:
              
        vector<AttributeType> buffer;      
        
        //! Number of distinct tuples 
        int nbDistTuples ;
        
        //! the tabular data   
        Data * table; 
        
        //! Used to get the numero of the attributes ( 1st, 2nd,...).
        RecodeToInt< AttributeType > numAttrib ; 
                
            
    public:  
                    
        //! Constructor
        template< class InputDBFormat >
        Key_base( Data & intable, InputDBFormat & input )
        { 
                  table = & intable ; 
                  numAttrib(input.attributes) ;
                  
                  nbDistTuples =0 ;
                  PTree<int, bool > tuplesProjection;
                  
                  for( typename Data::iterator it= table->begin(); it != table->end(); ++it )
                  {
                       tuplesProjection.push_back( *it );
                  }
          
                  nbDistTuples = tuplesProjection.size();
                  
        }
 
        //! Destructor
        ~Key_base(){}            
        
        //! Operator that test if a set of attributes is not redundant
        template< class Iterator, class Measure >
        bool operator() ( Iterator  itemsetIt, Measure & mesCand );
                 
};   


//! function used to count the projection of a candidates pointed by the iterator passed in parameter.            
/*!
    \param itCand iterator on the candidate to test.
    \return the cardinality of the projection.
*/ 
template< class Data, class AttributeType  >  template< class Iterator  >                  
int Key_base<Data,AttributeType>::countProjection( Iterator   itCand ) 
{
     PTree<int, bool > tuplesProjection;
     
     for_each( table->begin(), table->end(), ProcessTuples<Iterator>( &itCand, &tuplesProjection,&numAttrib  ) );

     return   tuplesProjection.size()  ;     
}



//! Operator that test if a set of attributes is a super key
/**
    \param itCand iterator (or pointer) on the set of attributes to test wrt the predicate
    \param mesCand cardinality of the projection on the db of the attributes
*/
template< class Data, class AttributeType  > template< class Iterator, class Measure>
bool Key_base<Data,AttributeType >::operator() ( Iterator    itCand, Measure & mesCand )
{ 
     
    
    // Count the cardianlity of the projection wrt the attributes in the candidate tested
    int cardProjectCand = countProjection(  itCand );   
     
    if( cardProjectCand !=  nbDistTuples ) // we have to process a single attribute
        return false;
        
    return true;                                                                                        
}

        
#endif 
