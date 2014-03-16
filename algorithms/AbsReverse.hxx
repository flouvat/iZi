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

#ifndef ABSREVERSE_HXX
#define ABSREVERSE_HXX

#include <set>

#include "AprioriReverse.hxx"
#include "NoOutput.hxx"
#include "Abs.hxx"

//! Functor finding the negative border Bd- and/or the positive border Bd+ using the algorithm ABS doing a top dow exploration of the search space.
/**
   The algorithme ("()" method)take in parameter:
        the initialisation functor, 
        the predicate,
        the functor determining at which iteration the levelwise step is stop,
        the positive and/or negative borders (optional),
        the transformation function (optional).
   
   The method executeAlgorithm could be use to execute the algorithm. 
   This method have another parameter to deal with "almost interesting elements" 
   based on an error function passed i n parameter.
   These elements are considered as interesting during the iterations, and at the end
   they are used to do a top-down traversal of the search space to find the latests elements of Bd+.
        
   The template parameter SetsType represents the type of the elements of the set representation.
   The template parameter Measure is the type of the value eventually associated with the word of the language.
   The template parameter Word represents the type of the elements of the language.       
   The template parameter Cand_DataStruct is the data structure type used by the algorithms to manipulate the candidates generated.
*/
template< class SetsType=int, class Measure=Boolean, class Word = vector< SetsType > ,  class Cand_DataStruct = PTree<SetsType,Measure > >
class AbsReverse: public Abs<SetsType,Measure,Cand_DataStruct>
{         
         //! Class used when there is no error functor
        class NoError
        {
            public:           
                template< class Iterator>        
                bool operator() ( Iterator it ) {return false;}                   
        };

    public:
    
    	//! Constructor.  
    	AbsReverse():Abs<SetsType,Measure,Cand_DataStruct>(){ }
    
    	//! Destructor.
    	~AbsReverse(){ }
	
        //! Functor operator that executes the algorithm.
        /**
           \param init functor initializing the interesting items wrt predicate
           \param pred the predicate 
           \param stopDualization functor determining when the apiori execution must stop 
           \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
           \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
           \param wordToset transformation function
        */
        template< class InitFunctor, class Predicate, class StopIteration, class OutputBdP, class OutputBdN, class f >   
        void operator() ( InitFunctor & init, Predicate  & pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN, f & wordToSet  ) 
        {
             Complement< SetsType > complem;
             TransformElement< Complement< SetsType >, SetsType, f, Word > trans( &wordToSet, &complem ) ;
             
            this->executeAlgorithm( init, &pred, stopDualization, outBdP, outBdN, (NoError * ) 0, trans ) ;             
        }    
        
        //! Functor operator that executes the algorithm.
        /**
           \param init functor initializing the interesting items wrt predicate.
           \param pred the predicate 
           \param stopDualization functor determining when the apiori execution must stop 
           \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
           \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
        */
        template< class InitFunctor, class Predicate, class StopIteration, class OutputBdP, class OutputBdN  >   
        void operator() ( InitFunctor & init, Predicate  & pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN ) 
        {
            Complement< SetsType > complem;
            this->executeAlgorithm( init, &pred, stopDualization, outBdP, outBdN, (NoError * ) 0, complem ) ;             
        }  
      

      
};




#endif
