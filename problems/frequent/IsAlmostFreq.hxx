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
 
#ifndef ISALMOSTFREQ_HXX
#define ISALMOSTFREQ_HXX


//! Determine if a not frequent itemset is near the positive border in the ABS algorithm
/**
    In ABS, elements near the positive border are considered as frequent until the end of algorithm.
    The these "almost frequent" elements are used to find the underling elements of the positive border.
*/
class IsAlmostFreq
{
    protected:
        
        //! threshold used to determine when an element is "near" the positive border
        double epsilon;    
        
        //! Minimum  support thresold
        int minsup;
        
    public:
             
        
        //! Constructor
        IsAlmostFreq( int inminsup, double  inepsilon ){ minsup = inminsup;  epsilon= inepsilon; } 
        
        //! Determine if a not frequent itemset is "near" the positive border
        /**
            A not frequent element X is "near", if (1 -   supp( X ) /minsup < thresold   
            \param itElement iterator on the element to study            
        */
        template< class Iterator >
        bool operator () ( Iterator  itElem )
        {
            double supp = itElem.measure().supp ;
            double error = 1- supp/minsup ; 
            return error < epsilon ;                        
        }        
};
    
#endif
