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
 
#ifndef IND_HPP 
#define IND_HPP

#include <iostream>
#include <string>


//! Class representing the inclusion between two attributes
class UnaryIND
{
      public:

            //  left side of the inclusion
            string left;
            
            //! right side of the inclusion
            string right ;  
            
            //! Default constructor
            UnaryIND(){ }          
           
            //! Constructor
            UnaryIND( string inleft, string  inright ){ left = inleft ; right = inright ; }          
      
            //! Copy constructor
            UnaryIND( const UnaryIND & indi){ left = indi.left; right = indi.right; }
                               
            //! Operator = overloading
            UnaryIND & operator = ( const UnaryIND & indi ){left = indi.left; right = indi.right; }  
            
            //! Equality operator overloading
            bool operator == (const UnaryIND & indi )const{ return( indi.left == left &&  indi.right == right) ; }                                      

            //! Inequality operator overloading
            bool operator != (const UnaryIND & indi )const{ return( indi.left != left ||  indi.right != right) ; }                                                  
            
            //! Less than operator overloading
            bool operator < ( const UnaryIND & indi ) const
            {
                 if( indi.left > left )
                     return false;
                 else if(  indi.left == left )
                      return indi.right < right ;
                 
                 return true ; 
            }
};   

ostream & operator<<( ostream & os,  UnaryIND & inUnaryIND ) 
{
        os << inUnaryIND.left <<" [ "<< inUnaryIND.right <<" ";                       
        return os; 
}      
      
//! Class representing a IND
class IND
{   
      
 public:
        
        //! left side of the IND
        vector< string > left;

        //! right side of the IND
        vector< string > right;
        
        //!Return the size of the IND
        int size(){ return left.size();}
      
};

ostream & operator<<( ostream & os,  IND & inIND ) 
{
        for( vector< string >::iterator it = inIND.left.begin(); it!= inIND.left.end(); ++it)
             os << *it <<" ";
             
        os <<"[ ";
        
        for( vector< string >::iterator it = inIND.right.begin(); it!= inIND.right.end(); ++it)
             os << *it <<" ";               
        
        return os; 
}


#endif
