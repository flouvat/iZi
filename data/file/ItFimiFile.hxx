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
 
#ifndef ITFIMIFILE_HXX
#define ITFIMIFILE_HXX

#include <vector>
#include <stack>
#include <iterator>

using namespace std;

template<class SetsType> class  FimiFile;

//! Iterator on a FimiFile.
/**
   The template parameter Container is the container use to store the data.
   \see FimiFile.
*/
template< class SetsType= int  >
class ItFimiFile 
{               
    protected:
           
        //! current FimiFile
        FimiFile<SetsType> * fimiFile;
           
        //! container with the current Data.
        vector<SetsType>  currentData;       
                                                       
     public:
          
          //! Contructor.
          ItFimiFile( FimiFile<SetsType> * ffile=0 ){ fimiFile = ffile; if( ffile ) fimiFile->readLine(currentData) ;} 
                                    
          //! Copy contructor.         
          ItFimiFile( const ItFimiFile & it ){ fimiFile= it.fimiFile; currentData= it.currentData;   }
    
          //! Affectation operator.
          ItFimiFile & operator = ( const ItFimiFile & it ){fimiFile= it.fimiFile; currentData= it.currentData; }
          
          //! Destructor.
          ~ItFimiFile(){};
          
          //! Increment operator.
          /**
             get the next data stored.
          */
          ItFimiFile &  operator++()
          { 
                     if( !fimiFile->readfile.is_open() ) fimiFile->readfile.open( fimiFile->fileName);   // open the input file                 
                     if (!fimiFile->readfile.is_open()) cerr<<"cannot open file "<< fimiFile->fileName<<endl; 

                     currentData.clear() ;               
                     fimiFile->readLine(currentData); 
                     return (*this); 
          };  
          
          //! Increment operator.
          /**
             get the next data stored.
          */
          ItFimiFile  operator++(int) 
          { 
                     if( !fimiFile->readfile.is_open() ) fimiFile->readfile.open( fimiFile->fileName);   // open the input file 
                     if (!fimiFile->readfile.is_open()) cerr<<"cannot open file "<< fimiFile->fileName<<endl; 

                     currentData.clear() ; 
                     fimiFile->readLine(currentData); 
                     return ItFimiFile(*this); 
          };        
          
          //! Operator returing the transaction pointed by the iterator
          vector< SetsType > & operator * () { if(!currentData.empty()) return currentData; } ;
                        
          //! Operator returing a pointer on the transaction pointed by the iterator                        
          vector< SetsType > * operator -> () { if(!currentData.empty() ) return &currentData; } ;    
          
          //! Equality operator 
          bool operator == ( const ItFimiFile & it ) const {  return ( fimiFile == it.fimiFile && currentData == it.currentData ); }

          //! Difference operator 
          bool operator != ( const ItFimiFile & it ) const { return ( fimiFile != it.fimiFile && currentData != it.currentData ); }
          
              
          friend class FimiFile<SetsType>;

};


#endif
