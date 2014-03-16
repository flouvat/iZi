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

#ifndef SUPPORTDISJ_HPP
#define SUPPORTDISJ_HPP

#include "Support.hpp"

//! Class used to store additional informations in each node, here the support and the disjunction of the corresponding items.
/**
    The disjunction is the number of transactions intersecting an itemset.
*/
class SupportDisj: public Support
{      
      public :

            //! store the disjunction of the corresponding itemset            
            int disj ;
    
    
            //! The constructor by  deault, in cunjonction with the operator != to mark that an element is in a node
            /*!
                An element is in in the node is the Support of the current node is != of Support(). 
            */
            SupportDisj():Support(){ disj= 0; } 
            
            SupportDisj( bool inpresence, int insupp = 0, int indisj=0 ):Support(inpresence,insupp){ disj=indisj; }
            
            SupportDisj( const SupportDisj & s ) { this->presence = s.presence; this->supp = s.supp ; this->disj = s.disj ; }

            SupportDisj & operator = ( const SupportDisj & s ) { this->presence = s.presence; this->supp = s.supp ; this->disj = s.disj ; return *this;}

                        
            bool operator != (const SupportDisj & s )const{ return( presence != s.presence || supp!=s.supp  || disj != s.disj) ; }                          

            bool operator == (const SupportDisj & s )const{ return( presence == s.presence && supp==s.supp && disj == s.disj ) ; }                                      
            
             //*****************************          
            // Specific of support counting
            //*****************************
                                  
            bool operator >= ( int i ) {return supp>= i ; }
                             
};

SupportDisj operator + ( const SupportDisj & s1, const SupportDisj & s2 ) { return SupportDisj( s1.presence + s2.presence, s1.supp + s2.supp  ) ;}  

ostream & operator<<( ostream & os, const SupportDisj & inval ) { os <<" ("<<inval.supp<<" | "<<inval.disj<<") "; return os; }

#endif
