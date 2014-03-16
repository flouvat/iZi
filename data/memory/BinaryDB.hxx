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
 
#ifndef BINARYDB_HXX
#define BINARYDB_HXX

#include <vector>
#include <iterator>
#include <time.h>

#include "ItBinaryDB.hxx"
#include "RecodeToInt.hxx"
#include "Tatree_base.hxx"
#include "Tatree.hxx"

using namespace std;

//! BinaryDB_base represents a binary relation, a transactionnal data bases.
/*! 
    This class is used to store in memory the transactional database for frequent itemsets mining.
    By default, the transactions are stored in a trie data structure (Tatree).   
    First, the transactions are stored in a temporary data structure (matrix).
    From this structure, the frequent items are extracted and reordered.
    After this, the transactions are recontructed (without not frequent items and in increasing oreder of support)
    and inserted in the final data structure.
                
    The template parameter SetsType is the type of the elements stored in the db.
    The template parameter DataStructDB is a container of sets to store the transactions in memory.   
*/
template< class SetsType, class DataStructDB=Tatree_base<SetsType>  >
class BinaryDB
{
    protected:

        //! Container used to store temporarly the transactions.
        vector< vector<SetsType> > * tmpDB;
        
        //! This container store the transactionnal database with ordered transactions of frequent items.
        DataStructDB db;  
               
    
    public:
           
        bool verbose ; // to print to screen    
                  
        //! Used to have the same syntax that the STL.
        typedef ItBinaryDB< SetsType, DataStructDB> iterator ;   
              
        //! Constructor.
        /**
            The constructor transfer the data from the input stream to a temporay data structure.    
            The template parameter is a stream manipulator that input the data from the db.
        */
        template< class InputFormat >
        BinaryDB( InputFormat & format, bool inverbose = false )
        {  
            tmpDB = new vector< vector<SetsType> > ; 
            
            clock_t start = clock();
       
            format >> *tmpDB;

            verbose = inverbose ;
 
            if( verbose )
            {
              cout<<"reading file ["<<(clock()-start)/CLOCKS_PER_SEC<<"s]"<<endl;
              cout<<endl;
            }
      
        }
        
        //! Destructor.
        ~BinaryDB() { if( tmpDB ) delete tmpDB ; } 
        
        //! Push back a set of element.
        /**
            This function insert a transaction in the final data structure.
            With default parameter, this function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of element of type SetsType.
            \param setElement the set of element to insert in the trie
            \param nb the number of times the set is inserted
        */             
        template< class Container > 
        void push_back(  Container& setElement, int nb = 1 )
        {                                 
             db.push_back( setElement, nb );
            
             // if all the transactions of the temporary databse had been inserted in the final one
             // then delete the temporary db
             if( tmpDB && tmpDB->size() == db.size() )
             {
                 delete tmpDB;
                 tmpDB = 0 ;
             }          
        }
             
        //! Push back a set of element.
        /**
           This function insert a transaction in the final data structure.
           With default parameter, the template parameter represents the input iterators on the elements to insert in the data base.
           \param first iterator on the first element
           \param last iterator on the element after the last
           \param nb the number of times the set is inserted
        */             
        template< class InputIterator> 
        void push_back( InputIterator & first, InputIterator & last, int nb = 1  )
        {   
            db.push_back( first,last, nb);
            
            // if all the transactions of the temporary databse had been inserted in the final one
            // then delete the temporary db
            if( tmpDB && tmpDB->size() == db.size() )
            {
                delete tmpDB;
                tmpDB = 0 ;
            }
            
        }         
        
        //! Set the recode function in the reconstructed data
        void setRecode( RecodeToInt<SetsType> * inrecode ) { db.setRecode(inrecode); }
        
        //! Return an iterator on the first element stored. 
        ItBinaryDB<SetsType,DataStructDB> begin() { return ItBinaryDB<SetsType,DataStructDB>( this );  }
            
        //! Return an iterator on the element after the last element stored. 
        ItBinaryDB<SetsType,DataStructDB> end() { return ItBinaryDB<SetsType,DataStructDB>() ; }  
                           
                          
        friend class ItBinaryDB<SetsType,DataStructDB>;    
        
        //! Return the data  
        DataStructDB & data() { return db;}  
};    


#endif
