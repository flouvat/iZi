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
 
#ifndef NOOUTPUT_HXX
#define NOOUTPUT_HXX


//! class used to not store/output a solution of the algorithm.
/**
    Using this class and the variable none the user select no output for a solution of the algorithm.
    For example if the theory parameter is set to "none" the algorithm will not store the theory.
*/
class NoOutput
{
    public:
        template< class InputIterator, class Measure>
        void push_back( InputIterator  first, InputIterator last,  Measure measure= Measure()  ){}
        
        template< class Container, class Measure >
        void push_back(  Container& setElement, Measure measure = Measure()  ){};      

};  
NoOutput * none =0 ;

#endif
