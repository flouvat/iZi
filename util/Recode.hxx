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

#ifndef RECODE_HXX
#define RECODE_HXX

#include <iterator>
#include <map>
#include <vector>
#include <list>


using namespace std;



//! Mother abstract class to recode elements of type DataType to type InternalType.
template<class DataType,class InternalType>
class Recode 
{
           
     protected:

        //! Map the original id with the internal id.
        map<DataType,InternalType> idTointernid ;   
        
        //! Map the internal id with the original id.
        map<InternalType,DataType>  internidToid ;  
      
     public:
      
        //! Constructor.         
        Recode(){ } 
        
        //! Destructor.
        virtual ~Recode(){ } 
        
        //! Return the object that transforms elements into internal id.
   	    map<DataType,InternalType> * getIdTointernid(){ return &idTointernid;}
        
        //! Return the object that transforms internal id into original elements.
        map<InternalType,DataType> * getInternidToid(){ return &internidToid;}         
        
        //! Map a set of elements of the input data in internals ids
        /**
            \return the recoded set.
        */
        template< class Container >     
        vector<InternalType> &  operator() ( Container  & itemset ){}      
        
        //! Map an element of the input data in an internal id
        /**
            \return the new id.
        */
        virtual InternalType  operator() ( DataType  item ) = 0 ;  
        
        //! Return the number of elements recoded.
        int size(){ return idTointernid.size() ;}
      
};



#endif
