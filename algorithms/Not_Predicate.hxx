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
 
#ifndef NOT_PREDICATE_HXX
#define NOT_PREDICATE_HXX

#include <functional>

#include "Predicate.hxx"

//! Predicate that return the negation of a predicate passed in parameter.
template< class PredicateNeg>
class Not_Predicate:public Predicate
{
    protected:
        
        //! Predicate to studied.
        PredicateNeg * pred;
        
    public:
        
        //! Constructor.
        Not_Predicate( PredicateNeg & inPred ) { pred = & inPred ;}
        
        //! Operator that test if a set of attributes is not redundant
        template< class Iterator, class Measure>
        bool operator() ( Iterator  itCand, Measure & mesCand ){ return ! (*pred)( itCand, mesCand) ;  }
        
        //! Function used to do some pre processing operations before testing the candidates generated.
        /*!
            \param cand container of words of the language.
        */
        template< class Cand_DataStruct, class f >
        void preProcessing( Cand_DataStruct & cand, f& wordToSet ){ pred->preProcessing( cand, wordToSet ) ; }   
        
        //! Function used to do some post processing operations after testing the candidates generated.
        /*!
            \param cand container of words of the language.
        */
        template< class Cand_DataStruct, class f>
        void postProcessing( Cand_DataStruct & cand, f& wordToSet ){ pred->postProcessing( cand, wordToSet ) ;}  
              
      
};

#endif
