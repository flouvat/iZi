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

#ifndef INITIND_HXX
#define INITIND_HXX

#include <utility>
#include <deque>

#include "IND.hpp"
#include "RecodeToInt.hxx"

    
//! Functor used to intialize the IND mining.
/**
   This functor is used to search the attributes of the relations and to construct the initial set unary INDs. 
   
   The template parameter InputDBFormat is the type of the database.
*/
template< class InputDBFormat >
class InitIND
{
    
    protected:
               
        //! The first input relational data
        InputDBFormat * input1 ;     
        
        //! The second input relational data
        InputDBFormat * input2 ; 
           
    public:
           
        //! list of the unary IND
        deque<IND> listElements;   
                
        //! Constructor
        InitIND(  InputDBFormat &  ininput1, InputDBFormat &  ininput2  ){ input1 = &ininput1; input2 = &ininput2; }     
  
        //! Execute the initialization of the basic word of the language (the di of size 1) in the algorithm
        void operator() ();         
};

//! Execute the initialization of the basic word of the language (the di of size 1) in the algorithm
template<  class InputDBFormat > 
void InitIND<InputDBFormat>::operator() ( )
{
    for( int i = 0; i< input1->attributes.size(); ++i)
    {
         string left =input1->attributes[i] ;
         
         for( int j = 0; j< input2->attributes.size(); ++j)
         {
              IND elemdi;
              elemdi.left.push_back( left );
              elemdi.right.push_back(  input2->attributes[j] );
              listElements.push_back( elemdi );         
         }
    }
}    





#endif
