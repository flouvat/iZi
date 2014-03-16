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

#ifndef INITKEY_HXX
#define INITKEY_HXX

#include <utility>
#include <deque>

#include "RecodeToInt.hxx"

    
//! Functor used to intialize the not redundant mining.
/**
   This functor is used to search the attributes of the database. 
   
   The template parameter AttributeType is the type of the attributes.   
   The template parameter InputDBFormat is the type of the database.
*/
template< class AttributeType, class InputDBFormat >
class InitKey
{
    
    protected:
               
        //! The input relational data
        InputDBFormat * input ;     
           
    public:
           
        //! list of the attributes
        deque<AttributeType> listElements;   
                
        //! Constructor
        InitKey(  InputDBFormat &  ininput  ){ input = &ininput; }     
  
        //! Execute the initialization of the basic word of the language in tha algorithm
        void operator() ();         
};

//! Execute the initialization of the basic word of the language in tha algorithm
template< class AttributeType, class InputDBFormat > 
void InitKey< AttributeType, InputDBFormat>::operator() ( )
{

     if(  ! listElements.size() )                 
        for( int i = 0; i< input->attributes.size(); ++i)
                listElements.push_back(input->attributes[i]);         
}    





#endif
