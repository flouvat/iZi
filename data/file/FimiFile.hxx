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
 
#ifndef FIMIFILE_HXX
#define FIMIFILE_HXX

#include <fstream>
#include <string>
#include <sstream>

#include "ItFimiFile.hxx"


//! Class used to read a transactionnal db in the FIMI format and save data in the FIMI format.
/**  
   This class is implemented uidung the stream classes of the iostream library.
   Consequently, it is less efficient thant the correspondig C class.  
   The template parameter SetsType is the type of items in the transactions.
*/
template< class SetsType=int>
class FimiFile 
{
              
   protected:
             
        //! Function reading a line of integer in the file.
        /**
           \param line stores the line of integers in a vector.
           \return false if end of file  
        */
        bool readLine(  vector<int> & line  );
           
      
        //! Function reading a line in the file.
        /**
           \param line stores the line (must have a push_back function).
           \return false if end of file  
        */
        template<class Container >
        bool readLine( Container & line);
              
              
        //! Function reading all the data in the file and store it in a container.
        /**
           \param container class representing the relation (must have a push_back function). 
           \return true if data has been read.
        */
        template< class ContainerDB >
        bool read( ContainerDB & container );
        
        //! Function write data in a file.
        /**
           \param begin iterator on the first element of the container to save
           \param supp the support of the itemset.
        */
        template< class Iterator, class Val >
        void write(Iterator begin, Iterator end, Val  measure ) ;
            
        
        //! Functor used to save in the file each element of a set
        class saveElemInFile
        {
            ofstream * f ;
            
            public:
                 
                 saveElemInFile( ofstream * inFile ) { f = inFile ; }                
                                   
                 template< class T >     
                 void operator() (T & x) { 
                      (*f)<<x<<" ";  
                      f->flush();                                        
                 }
        };
        
        //! Functor used to save in the file each set
        class saveContainerInFile 
        {
            ofstream * f;
            
            public:
                 
                 saveContainerInFile( ofstream * inFile ){ f = inFile; } 
                                                           
                 template< class Container >     
                 void operator() ( Container & cont )
                 {
                      saveElemInFile save(f) ; 

                      for_each( cont.begin(), cont.end(), save  );                        
                 }
        };
        

                  
         //! name of the file to used
        const char * fileName;
        
        //! Stream used to read in the file
        ifstream readfile;
        
        //! Stream used to write in the file
        ofstream writefile;      
                  
    public:
           
        //! Used to have the same syntax tha the STL 
        typedef ItFimiFile< SetsType > iterator ;              
        
        //! Constructor.
        FimiFile( const char * inFileName){ fileName= inFileName; };                                 
        
        //! Destructor.
        /**
           The destructor close the file.
        */  
        virtual ~FimiFile(){  if (!readfile.is_open() ) readfile.close(); if (!writefile.is_open() ) writefile.close(); }
               
        //! Extract data of the file and insert it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \return *this.
        */   
        template< class ContainerDB >
        FimiFile& operator>> ( ContainerDB & container )
        { 
            if (!readfile.is_open() )
                    readfile.open( fileName);       // open the file 
                              
            while( !readfile.eof() ) read( container) ; 
            return *this; 
        }    
        
        //! Extract all the  data of the container and insert it in a file .
        /**
           \param container where the data is read.     
           \return *this.
        */   
        template< class ContainerBdT >
        FimiFile& operator<< ( ContainerBdT & container )
        { 
            if (!writefile.is_open() )   
               writefile.open( fileName);       // open the file 
                                               
            saveContainerInFile save( &writefile );
            
            for( typename ContainerBdT::iterator it = container.begin(); it != container.end();++it )
            {
                save( *it );                
                writefile<< it.measure()<<"\n"  ;                         
            }    
            return *this;
        }
        
        FimiFile& operator<< ( char* str )
        { 
            if (!writefile.is_open() )   
               writefile.open( fileName);       // open the file 
                                               
            writefile<< str  ;                         
            
            return *this;
        }
        
        FimiFile& operator<< ( const char* str )
        { 
            if (!writefile.is_open() )   
               writefile.open( fileName);       // open the file 
                                               
            writefile<< str  ;                         
            
            return *this;
        }
        
        FimiFile& operator<< ( int i)
        { 
            if (!writefile.is_open() )   
               writefile.open( fileName);       // open the file 
                                               
            writefile<< i  ;                         
            
            return *this;
        }
        
        FimiFile& operator<< ( vector<SetsType> & v )
        { 
            if (!writefile.is_open() )   
               writefile.open( fileName);       // open the file 
            int size = v.size();
            for( int i = 0; i < size -1; ++i )
                writefile<<v[i]<<" ";      
            
            if( size ) writefile<<v[size-1];                  

            return *this;
        }         
        
             
        //! Insert an itemset in the file.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of integers.
            \param setElement the set of element to insert in the file.
            \param supp the support of the itemset.
        */             
        template< class Container, class Val >
        void push_back(  Container & setElement, Val  supp   )
        { 
                 if (!writefile.is_open() )
                        writefile.open( fileName);       // open the file 
               
                 saveElemInFile save( & writefile ) ;
                 for_each( setElement.begin(), setElement.end(), save ) ; // save the itemset
            
                 writefile<< supp <<"\n"; 
        }
        
        //! Insert an itemset in the file.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of integers.
            \param setElement the set of element to insert in the file.
            \param supp the support of the itemset.
        */             
        template< class Container >
        void push_back(  Container & setElement  )
        { 
                 if (!writefile.is_open() )
                        writefile.open( fileName);       // open the file 
               
                 saveElemInFile save( & writefile ) ;
                 for_each( setElement.begin(), setElement.end(), save ) ; // save the itemset
            
                 writefile<<"\n"; 
        }
        
        //! Insert a set of element in the file.
        /**
            The template parameter represents the input iterators on the integers to insert in the file.
            \param first iterator on the first element. 
            \param last iterator on the element after the last. 
            \param supp the support of the itemset.
        */             
        template< class InputIterator,  class Val>
        void push_back( InputIterator  first, InputIterator last, Val  supp   )
        {       
                 if (!writefile.is_open() )
                        writefile.open( fileName);       // open the file 
                        
                 saveElemInFile save( & writefile ) ;
                 for_each( begin, end, save ) ; // save the itemset
            
                 writefile<< supp <<"\n"; 
        }
                         
        //! Return an iterator on the first element stored. 
        ItFimiFile< SetsType > begin() {  return ItFimiFile< SetsType >( this );  };
            
        //! Return an iterator on the element after the last element stored. 
        ItFimiFile< SetsType > end() { return ItFimiFile< SetsType >() ; } ;  
        
        //! Close the file
        void close(){ if (!readfile.is_open() ) readfile.close(); if (!writefile.is_open() ){ writefile.close();} }     
                           
        friend class ItFimiFile< SetsType >;  
         

};

//! Function reading a line in the file.
/**
   \param line stores the line (must have a push_back function).   
   \return false if end of file  
*/
template<class SetsType> template<class Container > 
bool FimiFile<SetsType>::readLine( Container & line  )
{ 
    string::size_type oldpos ;
    string::size_type pos ;
  
    if (!readfile.is_open() )
            readfile.open( fileName);       // open the file 
  
    if( readfile.is_open() && !readfile.eof()   )
    {  

        string buffer, buffer2;
        
        if( getline( readfile, buffer,'\n' ) && buffer!= "\n" )// read one line in the file
        {   
            oldpos= 0 ;
            pos=0;   
            int cptAttrib= 0 ; // number of the elements 
                
            while(  (pos = buffer.find(" ", oldpos ) )!=  string::npos && buffer!="\n" )
            {   
                    stringstream ss( buffer.substr( oldpos, pos-oldpos ) );

                    SetsType elem;                       
                    ss >> elem ;
                    line.push_back( elem );
   
                    oldpos =pos+1;
                    ++cptAttrib;             
            }   
            
            if( cptAttrib )
            {
                pos = buffer.find("\n")  ;  
                
                stringstream ss( buffer.substr( oldpos, pos-oldpos ) );

                SetsType elem;                       
                ss >> elem ;
                    
                line.push_back(  elem );
                return true;                                    
            }

        }    
    } 
   
    return false;   
}    

//! Function reading a line of integer in the file.
/**
    \param line stores the line of integers in a vector.
    \return false if end of file  
*/
template<class SetsType > 
bool FimiFile<SetsType >::readLine( vector<int> & line  )
{

    if (!readfile.is_open() )
            readfile.open( fileName);       // open the file 
    
    if( readfile.is_open() && !readfile.eof()   )
    {    
                                               
        char c;  
        int item=0, pos=0; 
        
        do
        {     
            item=0, pos=0;                  
            readfile.get( c );
                          
            while( !readfile.eof() && ( c >= '0' ) && ( c <= '9' ) )
            {
                  item *=10;
                  item += int(c)-int('0');
                  readfile.get( c );
                  pos++;
            }
            
            if(pos)
                  line.push_back( item );                                    

        
   
        }while( c != '\n' && !readfile.eof() );
       

        return true;  
            
    } 
   
    return false;   
}    

//! Function reading data in a file and store it in a container.
/**
   \param container where the data read is inserted (must have a push_back function).     
   \return  false if end of file 
*/
template<class SetsType>  template< class ContainerDB >
bool FimiFile<SetsType>::read( ContainerDB & container  )
{
    if (!readfile.is_open() )
        readfile.open( fileName);       // open the file 

    vector<SetsType> line; 
    bool data = readLine( line ) ;

    if( data  )
    {	            
            container.push_back( line );                         
    }
    return data;   
}    

#endif
