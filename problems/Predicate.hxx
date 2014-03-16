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
 
#ifndef PREDICATE_HXX
#define PREDICATE_HXX

using namespace std;

//! Mother abstract functor representing a predicate. 
/*!
    This functor test if an itemset is interesting wrt the predicate.
    The template parameter Iterator is an iterator on a candidate.
*/
class Predicate
{
           
    public:
        
        //! Constructor.
        Predicate(){} 
        
        //! Destructor
        virtual ~Predicate(){}
        
         //! Operator that test if a word of the language is interesting wrt predicate or not.
        /*!
            \param wordIt iterator on the word to test.    
        */
        template< class Iterator, class Measure >
        bool operator() ( Iterator   wordIt, Measure & mesCand ){ return (*this)( wordIt ); }
        
         
        //! Function used to do some pre processing operations before testing the candidates generated.
        /*!
            \param cand container of words of the language.
        */
        template< class Cand_DataStruct, class f >
        void preProcessing( Cand_DataStruct & cand, f  wordToSet ){}   
        
        //! Function used to do some post processing operations after testing the candidates generated.
        /*!
            \param cand container of words of the language.
        */
        template< class Cand_DataStruct, class f>
        void postProcessing( Cand_DataStruct & cand, f  wordToSet ){}   

};    



#endif 
