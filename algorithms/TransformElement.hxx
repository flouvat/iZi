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

#ifndef TRANSFORMELEMENT_HXX
#define TRANSFORMELEMENT_HXX

#include <vector>

//! Functor used to transform elements of the langugage.
/*!
    This class could be used to transform words of the language into sets
    and to eventually transform the sets.
    The template parameter WordToSet is the type of the function that transforms the language in sets.
    The template parameter TransformSet is the type of the function that transforms the sets.
    The template parameter SetsType is the type of the sets elements.
*/
template<  class TransformSet, class SetsType  , class WordToSet, class Word=vector< SetsType > >
class TransformElement 
{    
    public:   
                   
        //! Functor to transform a word into a set
        WordToSet * wordtoset;
        
        //! Functor to transform a set
        TransformSet * transformset ;     
                                                 
        //! Constructor
        TransformElement(  WordToSet * inwordtoset, TransformSet * intransformset ){ wordtoset = inwordtoset ; transformset = intransformset ;}                      
              
        //! Operator applying the transformations functions.
        /**
            \param w word to transform into set
        */
        template< class IteratorWord >
        vector<SetsType> * operator() ( IteratorWord   w )
        {            
            return  (*transformset)( (*wordtoset)( w ) ) ; 
        }
        
        //! Method returning original element.
        /**
            \param s set to transform in the origianl word
        */
        template<  class IteratorSet>
        Word *  inverse( IteratorSet  s ) 
        {                         
            return  wordtoset->inverse( transformset->inverse( s ) ) ; 
        }
        
};

#endif
