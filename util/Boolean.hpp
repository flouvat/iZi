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
 
#ifndef BOOLEAN_HPP
#define BOOLEAN_HPP
 
 #include<iostream>
 
//! Boolean type
/**
    This class is created since their is a problem with vector<bool>::iterator in the STL.
    It returns a temporary _Bit_Reference rather than a "proper" reference. 
    Vector<bool> isn't a proper container and it's iterators don't satisfy the conditions for
    forward iterators. 
    For more details see: http://gcc.gnu.org/ml/libstdc++/2005-03/msg00148.html
*/

class Boolean
{
      public:
             
            bool val; 
             
            Boolean(){ val = false; } 
            
            Boolean( bool inval ){ val=inval; }
            
            bool operator != (const Boolean & s )const{ return val != s.val  ; }                          

            bool operator == (const Boolean & s )const{ return val == s.val ; }                                      
            
            Boolean & operator ++ () { val = true; return(*this); }
            Boolean operator ++ ( int ) { return Boolean( true); }             
            
      
};

Boolean operator + ( const Boolean & s1, const Boolean & s2 ) { return Boolean( s1.val + s2.val) ;}  

ostream & operator<<( ostream & os, const Boolean & inval ) { os << inval.val; return os; }

 
#endif
