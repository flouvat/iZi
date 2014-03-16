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
 
#ifndef ITBINARYDB_HXX
#define ITBINARYDB_HXX

#include <vector>
#include <map>
#include <stack>
#include <iterator>

using namespace std;

template< class SetsType, class DataStructDB  > 
class BinaryDB;

//! Iterator on a BinaryDB.
/**
   The template parameter SetsType is the type of the element stored in the db.
   The template parameter DataStructDB is the type of the data structure storing the db.
   The template parameter Container is the container used to store the transactions.
   
   \see BinaryDB.
*/
template<class SetsType, class DataStructDB, class Container= vector<SetsType> >
class ItBinaryDB 
{ 
         typedef typename  vector<SetsType>::iterator ItVect ;         
         typedef typename  vector< vector<SetsType> >::iterator ItVectVect ;  
         typedef typename DataStructDB::iterator  IteratorDataStruct;           
                           
    protected:
          
         //! Iterator on the current transaction.
         IteratorDataStruct currentIt;  
         
         //! Iterator on the temporary db
         ItVectVect * iTtmpDB;
         
         //! Iterator on the end of thetransaction.
         IteratorDataStruct endIt;  
         
         //! Iterator on the end of the temporary db
         ItVectVect * endItTmp;                                                     
                                                                                                              
     public:
          
        //! Contructor.
        /**
            Initialize the iterators on the db or in the temporary db if the final does not exist.
        */
        ItBinaryDB( BinaryDB<SetsType, DataStructDB> * indb=0 )
        {   
                          
            iTtmpDB = endItTmp = 0 ;   
            if( indb )       
            {
                if( !indb->tmpDB )
                {
                    currentIt=(indb->db).begin();
                    endIt =(indb->db).end();
                }
                else
                {
                   
                    iTtmpDB = new ItVectVect( indb->tmpDB->begin() );                                                                         
                    endItTmp = new ItVectVect( indb->tmpDB->end() );                                                                         
                }
            }
        } 
                        
        //! Copy contructor.    
        ItBinaryDB( const ItBinaryDB & it )
        {                                                                       
            iTtmpDB = 0;
            endItTmp =0;
            if( it.iTtmpDB )
            {
                if( ! iTtmpDB )
                    iTtmpDB = new ItVectVect;
                    
                if( ! endItTmp )
                    endItTmp = new ItVectVect;   
                
                *iTtmpDB = (*it.iTtmpDB);                  
                *endItTmp = *(it.endItTmp);
            }    
            currentIt= it.currentIt; 
            endIt= it.endIt;                                                   
        }
        
        //! Affectation operator.
        ItBinaryDB & operator = ( const ItBinaryDB & it )
        {                  
            if( it.iTtmpDB )
            {
                if( ! iTtmpDB )
                {
                    delete iTtmpDB;
                    iTtmpDB = new ItVectVect;
                    delete endItTmp;
                    endItTmp = new ItVectVect;   
                }
                    
                *iTtmpDB = *it.iTtmpDB; 
                *endItTmp = *(it.endItTmp);
            }
            else 
            {
                if( ! iTtmpDB )
                {
                    delete iTtmpDB;
                    delete endItTmp;
                }
                iTtmpDB = endItTmp =0; 
            }    
            currentIt= it.currentIt; 
            endIt= it.endIt;       
        }
        
        //! Destructor.
        ~ItBinaryDB(){ if ( iTtmpDB ){ delete iTtmpDB; delete endItTmp;  }  }
        
        //! Increment operator.
        /**
            get the next set stored.
        */
        ItBinaryDB &  operator++()
        {   
            if ( iTtmpDB )
            {                                                               
                ++(*iTtmpDB); 
                
                if( *iTtmpDB == *endItTmp )
                {
                    delete iTtmpDB;
                    delete endItTmp ;
                    iTtmpDB = 0 ;
                    endItTmp =0 ;
                 }
            }                      
            return (*this); 
        }  
         
        //! Increment operator.
        /**
            get the next set stored.
        */    
        ItBinaryDB  operator++(int)
        {                                
            if ( iTtmpDB )            
            {                  
                ++(*iTtmpDB); 
                
                if( *iTtmpDB == *endItTmp )
                {
                    delete iTtmpDB;
                    delete endItTmp ;
                    iTtmpDB = 0 ;
                    endItTmp =0 ;
                 }
            }                      
            return (*this); 
        }

        //! Operator returning the transaction pointed by the current iterator.
        Container & operator * () 
        { 
            if ( iTtmpDB )
            {
               if( *iTtmpDB != *endItTmp )
                      return *(*iTtmpDB); 
			   
            }
			
        } ;
        
        //! Operator returning a pointer on the transaction pointed by the current iterator.            
        Container * operator -> () {  if ( iTtmpDB ) return &(*(*iTtmpDB)); } ;    
       
        //! Equality operator                        
        bool operator == ( const ItBinaryDB & it ) const 
        {                                     
             if( ! iTtmpDB && ! it.iTtmpDB )             
                 return ( currentIt == it.currentIt  ); 
             else if(  iTtmpDB &&  it.iTtmpDB )
                 return ( *iTtmpDB == *it.iTtmpDB );                 
             else return false ;
        }
        
        //! Difference operator                                
        bool operator != ( const ItBinaryDB & it ) const 
        {        
             if( ! iTtmpDB && ! it.iTtmpDB )               
                 return ( currentIt != it.currentIt ); 
             else if(  iTtmpDB &&  it.iTtmpDB )
                 return ( *iTtmpDB != *(it.iTtmpDB) );                 
             else
                  return true ;
        }
        
        friend class BinaryDB<SetsType,DataStructDB> ;
              
};

#endif
