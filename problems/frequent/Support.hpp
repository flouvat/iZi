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

#ifndef SUPPORT_HPP
#define SUPPORT_HPP

//! Class used to store additional informations in each node, here the support of the corresponding items.
class Support
{      
      public :
            //! used to not update the support of an itemset already considered
            bool presence ;
            
            //! store the support of the corresponding itemset
            int supp ;
    
    
            //! The constructor by  deault, in cunjonction with the operator != to mark that an element is in a node
            /*!
                An element is in in the node is the Support of the current node is != of Support(). 
            */
            Support(){ supp = 0 ; presence = false; } 
            
            Support( bool inpresence, int insupp = 0 ){ presence = inpresence; supp = insupp; }
            
            bool operator != (const Support & s )const{ return( presence != s.presence || supp!=s.supp ) ; }                          

            bool operator == (const Support & s )const{ return( presence == s.presence && supp==s.supp ) ; }                                      
            
            //*****************************          
            // Specific of support counting
            //*****************************
                              
            bool operator >= ( int i ) {return supp>= i ; }
            
            
///////////////////////////////////////////////////////////////////////////
            Support & operator + ( int i ) { supp = supp + i ; return *this ; }
            Support & operator = ( int i ) { presence = true; supp =  i ; return *this ; }
                       
};

Support operator + ( const Support & s1, const Support & s2 ) { return Support( s1.presence + s2.presence, s1.supp + s2.supp  ) ;}  

ostream & operator<<( ostream & os, const Support & inval ) { os << inval.supp; return os; }

#endif
