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

#ifndef INITIND_DBMS_HXX
#define INITIND_DBMS_HXX

#include <utility>
#include <deque>

#include "IND.hpp"
#include "RecodeToInt.hxx"

    
//! Functor used to intialize the IND mining.
/*!
    This functor is used to search the attributes of the relations and to construct the initial set unary INDs. 
       
    The template parameter DBMS is the class representing the connection to the DBMS.
    This class is used to do operations on the DBMS such as connect, execute a query ...
*/
template< class DBMS >
class InitIND_DBMS
{
    
    protected:
               
        //! Pointer on the dbms
        DBMS * mydbms;
        
           
    public:
           
        //! list of the unary IND
        deque<IND> listElements;   
                
        //! Constructor
        /*!
            \param inDbms pointer on the DBMS object to consider
        */
        InitIND_DBMS( DBMS * inDbms ){ mydbms = inDbms ; }     
  
        //! Execute the initialization of the basic word of the language (the di of size 1) in the algorithm
        void operator() ();         
};

//! Execute the initialization of the basic word of the language (the di of size 1) in the algorithm
template<  class InputDBFormat > 
void InitIND_DBMS<InputDBFormat>::operator() ( )
{
     
    if( mydbms->get_relation(1) && mydbms->get_relation(2) ) 
    {
      
        for( int i = 0; i<  ( mydbms->get_relation(1) )->attributes.size(); ++i) 
        {
             string left =( mydbms->get_relation(1) )->attributes[i] ; 
             
             for( int j = 0; j< ( mydbms->get_relation(2) )->attributes.size(); ++j)
             {
                  IND elemdi;
                  elemdi.left.push_back( left );
                  elemdi.right.push_back(  ( mydbms->get_relation(2) )->attributes[j] );
                  listElements.push_back( elemdi );                  
             }
        }
    }
       
}    





#endif
