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

#ifndef COMPLEMENT_HXX
#define COMPLEMENT_HXX

#include "RecodeToInt.hxx"

//! Functor that complements a set.
/*!
    The template parameter SetsType is the type of the elements.
*/
template< class SetsType>
class Complement 
{
        typedef typename vector<SetsType>::iterator ItVect;
    
        //! functor used to complement a set in a for_each (method of the STL).    
        class addToComplem
        {                                             
                vector<SetsType> *  complem ;
                
                vector<SetsType>  * listElements;  
                
                ItVect * itList ; 
                
                ItVect * end ;
                                              
            public:
                                                        
                addToComplem(ItVect & inFirstList, ItVect  inLastList, vector<SetsType> *  incomplem ) 
                { 
                    end = &inLastList ; 
                    itList = &inFirstList; 
                    complem = incomplem;
                }
                    
                void operator() ( SetsType & elem )
                {     
                     while( *itList != *end  )
                     {                                      
                        if( **itList != elem )
                        {
                            complem->push_back(**itList) ;    
                        }
                        else
                        {                                                        
                            ++ (*itList) ; 

                            break;
                        }
                        
                        ++ (*itList) ;                          
                     }                                          
                }             
        };
        
        //! container storing the complement of the set passed in parameter
        vector<SetsType>   complem ;  
                                    
    public:                
           
        //! list of elements of size 1
        /**
            It musts be initialized before using the functor.
        */
        vector<SetsType>  * listElements;   
        
        //! constructor        
        Complement(){ listElements = 0; }
               

        //! Operator returning the complement of a set.
        /**
            \param s set to complement
        */
        template< class IteratorSet >
        vector<SetsType> * operator() ( IteratorSet   s )
        {  
                
            complem.resize( 0 ) ;

            if( s->size() > 1 )
            {     
                           
                complem.reserve( listElements->size() - s->size() );
                 
                ItVect  itList = listElements->begin();                             
                addToComplem add( itList, listElements->end(), & complem ) ;
                
                // add the element not in s
                for_each( s->begin(), s->end() , add ); 
    
                // add the last elements of listElements
                while( itList != listElements->end()  )
                {
                       complem.push_back( *itList ) ; 
                       ++ (itList) ;
                }

            }
            else //to insert the attribute in the candidates set and not their complement
            {
                complem.resize( 1 );                
                complem[0] = * s->begin() ;               
            }

           
            return  &complem ; 
        }
        
        //! Method returning the complement of a set.
        /**
            \param s set to complement
        */
        template<  class IteratorSet >
        vector<SetsType> *  inverse( IteratorSet  s ) 
        {   
          
            complem.resize( 0 ) ;
            
            complem.reserve(listElements->size() - s->size());
            
            ItVect  itList = listElements->begin();
            addToComplem add( itList , listElements->end(), & complem ) ;
            
            // add the element not in s
            for_each( s->begin(), s->end() , add ); 

            // add the last elements of listElements
            while( itList != listElements->end()  )
            {
               
                   complem.push_back( *itList ) ; 
                   ++ (itList) ;
            }
            return  &complem ; 
        }
        

};

#endif
