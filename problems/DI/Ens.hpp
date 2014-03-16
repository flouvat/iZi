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
 
#ifndef ENS_HPP 
#define ENS_HPP

#include "IND.hpp"

// Class that process the representation as sets of a IND 
class Ens
{
       //! temporary IND used to transform the elements
       IND tmpIND ; 
       
       vector< UnaryIND > ensIND;
            
      public:
             
        //! Operator returning the set representing the IND passed in parameter.
        /**
            \param s iterator on the input IND
        */
        template< class IteratorIND >
        vector<UnaryIND> * operator() ( IteratorIND   s )
        {
            ensIND.resize( 0) ;
            ensIND.reserve( s->left.size() );
                             
            for( int i = 0; i < s->left.size() ; ++i )
                 ensIND.push_back( UnaryIND( s->left[i], s->right[i] ) );
                         
            return &ensIND;
        } 
        
        
        //! Method returning the IND represented by the set in parameter.
        /**
            \param s iterator in the set to transform in IND
        */
        template<  class IteratorSet >
        IND *  inverse( IteratorSet  s )
        { 
            tmpIND.left.resize( 0) ;  
            tmpIND.right.resize(0) ;

            tmpIND.left.reserve( s->size() ) ;
            tmpIND.right.reserve( s->size() ) ;            
                        
            for( vector< UnaryIND >::iterator it = s->begin(); it != s->end() ; ++it  )
            {
                 tmpIND.left.push_back( it->left) ;
                 tmpIND.right.push_back( it->right) ;                 
            } 
            
            return & tmpIND ;
        }      
      
};



#endif
