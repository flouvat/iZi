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
 
#ifndef RECORDDISTRIBUTION_HXX
#define RECORDDISTRIBUTION_HXX

#include "NoOutput.hxx"
#include <vector>

using namespace std;

//! Class used to store the distribution of a solution find by an algorithm (and evantually save the solution in an output object).
/**
    The objects of this class are passed in parameter of the algorithm by using the output parameters.
    For example, suppose that we have an algorithm that process the theory of a language.
    This algorithm will have at least 3 parameters: the initialization functor of the language, the predicate and an object to output the theory.
    This output object could be a file, a data structure or a "statistical" object that records some informations about the elements of the solution.
    The current class is such a "statistical" output class, and it is used to store the distribution of the elements of the solution, ie the number of elements  wrt to their size.    
    Since it should be possible to record stat and save solution in a file or a dat structure, this class could be a wrapper to such objects.
    This way this class could record informations and save solutions in the output object wrapped at the same time and transparently..
*/
template< class Output = NoOutput >
class RecordDistribution 
{
        //! Count the size of a container when only the iterator are passed in parameter                 
        class CountSize      
        {
            public:
                int  * size;
                
                CountSize( int & insize ){size=&insize;}
                
                template< class T>
                void operator()(T & x){  ++(*size); }               
        };
                                                  
    protected:
        
        //! distribution of the elements of the solution      
        vector<int> distrib;      
      
        //! Pointer on the ouput object wrapped
        Output * objectwrapped ;
                  
    public:
          
        //! Constructor.
        RecordDistribution( Output * inobjectwrapped= 0  ){ objectwrapped = inobjectwrapped ;}
        
        //! Destructor.
        virtual ~RecordDistribution(){}
        
        
        //! Function to access the container storing the ditribution
        vector<int> & distribution() { return distrib ;}
               
                    
        //! Function called when a solution is find, and used to store informations and eventually store the solution in another output object.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of elements.
            \param setElement the set of element to insert in the file.
            \param measure additional measure assoiated with the element.
        */             
        template< class Container, class Measure >
        void push_back(  Container & setElement, Measure measure )
        { 
             
            int size = setElement.size() ;
             
            if( objectwrapped ) 
                objectwrapped->push_back( setElement, measure );
             
            if( distrib.size() < size )
                distrib.resize( size, 0 ) ;
            
            ++( distrib[size-1] );                 
                    
        }
        
         //! Function called when a solution is find
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of elements.
            \param setElement the set of element to insert in the file.
        */             
        template< class Container >
        void push_back(  Container & setElement, Boolean measure )
        { 
             
            int size = setElement.size() ;
             
            if( objectwrapped ) 
                objectwrapped->push_back( setElement );
             
            if( distrib.size() < size )
                distrib.resize( size, 0 ) ;
            
            ++( distrib[size-1] );                 
                    
        }
        
         //! Function called when a solution is find, and used to store informations and eventually store the solution in another output object.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of elements.
            \param setElement the set of element to insert in the file.
            \param measure additional measure assoiated with the element.
        */             
        template< class Container  >
        void push_back(  Container & setElement  )
        { 

            int size = setElement.size() ;

             
            if( objectwrapped ) 
                objectwrapped->push_back( setElement  );

             
            if( distrib.size() < size )
                distrib.resize( size, 0 ) ;
            
            ++( distrib[size-1] );                 
     
        }
        
        //! Function called when a solution is find, and used to store informations and eventually store the solution in another output object.
        /**
            The template parameter represents the input iterators on the integers to insert in the file.
            \param first iterator on the first element. 
            \param last iterator on the element after the last. 
            \param measure additional measure assoiated with the element.
        */             
        template< class InputIterator,  class Measure>
        void push_back( InputIterator first, InputIterator last, Measure measure )
        {     
            int size  = 0 ;                    
            CountSize count( size );                      
          
            for_each( first, last, count );        
       
            if( objectwrapped ) 
                objectwrapped->push_back( first, last, measure );
             
            if( distrib.size() < size )
                distrib.resize( size, 0 ) ;
            
            ++( distrib[size-1] );                                
            
        }
        
        //! Get the number of levels studied.
        int size(){ return distrib.size(); }
        
        //! Access the number elements of size i
        int & operator[] ( const int & i ){ return distrib[i]; }
                                 
};

//! Screen output operator
template< class Output >
ostream & operator<<( ostream & os, RecordDistribution<Output> &  recDistrib ) 
{ 
    vector<int>  distrib =  recDistrib.distribution()  ;   
    
    for( int i = 0 ; i < distrib.size(); ++i )       
        os << ( distrib.at(i) )<<endl; 
    return os; 
}



#endif
