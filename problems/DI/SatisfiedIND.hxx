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
 
#ifndef SATISFIEDIND_HXX
#define SATISFIEDIND_HXX

#include<set>
#include<utility>

#include "Predicate.hxx"
#include "PTree.hxx"
#include "RecodeToInt.hxx"

//! Functor representing the predicate being a satisfied IND wrt to 2 relations. 
/*!
    This functor test if a IND is satisfied.  
    The method used to test if a candidate is a a satisfied IND is the following one:        
        * process the projection wrt the attributes of the left side of the candidate IND.
        * process the projection wrt the attributes of the right side of the candidate IND.
        * check that each element of left side projection is included in the right side projection.
        * return true if all the element are included.    
    
    The template parameter Data is the type of the tablular data.
*/
template< class Data>
class SatisfiedIND: public Predicate 
{
        
      
    protected:
                    
      
        //! function used to project a relation  wrt a set of attributes.
        /*!
            \param relation class representing the relation.
            \param ContainerAttrib set of attribute used for the projection.
            \param ContainerProject container used to store the projection.
        */          
        template< class ContainerAttrib, class ContainerProject  >                  
        void projection( Data * relation, ContainerAttrib & setOfAttrib, ContainerProject & project,  RecodeToInt< string > & numAttrib  ) ;
              
              
        //! Functor used to projet all the tuples wrt attributes studied 
        template< class ContainerAttrib, class ContainerProject >
        class ProcessTuples
        {            
                 //! set of attribute used for the projection
                 ContainerAttrib * setOfAttrib;      
                 
                 //! container used to store the projection
                 ContainerProject * project;
                 
                 //! return the number of the attributes of the relation
                 RecodeToInt< string > * numAttrib ;
                 
                 //! the projected tuple with the value of the attributes recoded
                 vector<int> tupleProjected;

            
            public:
                 ProcessTuples( ContainerAttrib & insetOfAttrib, ContainerProject & inproject,  RecodeToInt< string > & innumAttrib  )
                 { 
                         setOfAttrib = & insetOfAttrib;
                         project = & inproject ;
                         numAttrib = & innumAttrib ;
                 }
                 
                 template< class ContainerTuple >
                 void operator() ( ContainerTuple &  tuple)
                 {
                      vector<int> * setOfAttribNum = & (*numAttrib)(*setOfAttrib);

                      tupleProjected.resize( 0 );
                      tupleProjected.reserve( setOfAttribNum->size() );                      

                      for_each(  setOfAttribNum->begin(), setOfAttribNum->end(), project2<ContainerTuple>( tuple, tupleProjected) );

                      project->insert( tupleProjected  ) ;                          

                 }                                   
        };
       
        //! Functor used to projet a tuple wrt the set of attributes studied
        template< class ContainerTuple >
        class project2
        {
                 ContainerTuple * tuple ;
                 
                 vector<int> * tupleProjected ;     
                                 
            public:                     
                 project2( ContainerTuple & intuple, vector<int> & intupleProjected )
                 { 
                          tuple = &intuple;
                          tupleProjected = &intupleProjected; 
                 }
                 
                 void operator() ( int  attrib )
                 {
                      tupleProjected->push_back( tuple->at(attrib ) ) ; 
                 }                                            
        };       
       
        

    protected:
              
        vector<string> buffer;      
                
        //! the first relation 1
        Data * table1; 

        //! the first relation 2
        Data * table2; 
        
        //! Used to get the numero of the attributes ( 1st, 2nd,...) of the first relation.
        RecodeToInt< string > numAttrib1 ; 
                
        //! Used to get the numero of the attributes ( 1st, 2nd,...) of the first relation.
        RecodeToInt< string > numAttrib2 ; 

            
    public:  
                    
        //! Constructor
        /*!
            \param intable1 data of the first relation/table studied
            \param input1 file containing the first relation/table studied
            \param intable2 data of the second relation/table studied
            \param input2 file containing the second relation/table studied
        */
        template< class InputDBFormat >
        SatisfiedIND( Data & intable1, InputDBFormat & input1, Data & intable2, InputDBFormat & input2 )
        { 
                  table1 = & intable1 ; 
                  numAttrib1(input1.attributes) ;
                  
                  table2 = & intable2 ; 
                  numAttrib2(input2.attributes) ;
                  
        }
 
        //! Destructor
        ~SatisfiedIND(){}            
        
        //! Operator that test if a set of attributes is a satisfied IND
        template< class Iterator, class Measure >
        bool operator() ( Iterator itCand, Measure & mesCand );
                 
};   


      
//! function used to project a relation  wrt a set of attributes.
/*!
    \param relation class representing the relation.
    \param setOfAttrib set of attribute used for the projection.
    \param project container used to store the projection.
    \param recodedval used to recode the attributes the value in int
    \param numAttrib return the number of the attribute in the relation
*/          
template< class Data > template< class ContainerAttrib, class ContainerProject  >                  
void SatisfiedIND<Data >::projection( Data * relation, ContainerAttrib & setOfAttrib, ContainerProject & project, RecodeToInt< string > & numAttrib  ) 
{
     for_each( relation->begin(), relation->end(), ProcessTuples<ContainerAttrib,ContainerProject>( setOfAttrib, project, numAttrib ) );

}



//! Operator that test if a set of attributes is a super key
/**
    \param itCand iterator (or pointer) on the set of attributes to test wrt the predicate
    \param mesCand cardinality of the projection on the db of the attributes
*/
template< class Data > template< class Iterator, class Measure>
bool SatisfiedIND<Data >::operator() ( Iterator    itCand, Measure & mesCand )
{ 
    set< vector<int> >  proj1 ; // projection of each tuple of table1
    set< vector<int> >  proj2 ;
         
    // project the relation 1 wrt the attributes in the candidate tested
    projection( table1, itCand->left, proj1, numAttrib1 );   
    
     // project the relation 1  wrt the attributes in the candidate tested
    projection( table2,   itCand->right, proj2, numAttrib2 ); 
     
    if( includes( proj2.begin(), proj2.end(),proj1.begin(),proj1.end() ) ) // test if the di is satisfied
        return true;
        
    return false;                                                                                        
}

        
#endif 
