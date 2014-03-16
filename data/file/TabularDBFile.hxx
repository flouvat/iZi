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
 
#ifndef TABULARDBFILE_HXX
#define TABULARDBFILE_HXX

#include <fstream>
#include <string>

#include "Boolean.hpp"
#include "RecodeToInt.hxx"


//! Read a tabular db and save solution in a file in the FIMI format.
/**
    The input functions of this class read tabular data stored in a file having the following format: 
        - first line -> list of the attributes separadte by ";"
        - each line -> one tuple with the values separated by ";"
    The output functions of this class store the solutions in a file in the FIMI format:
        - one line -> an element (set of attributes) of the solution 
        - last value -> the cardinality of the projection of the attributres on the data 
        
    The values int the tuples are recoded to int when stored in memory to occupy less memory           
*/
class TabularDBFile 
{
      //! boolean to know if the memory for the recodeTuples attribute must localy dealloacte
      bool free_recode;
      
    protected:
      
        //! Function reading a tuple in the file.
        /**
           \param tuple stores the tuple (must have a push_back function).   
           \param inRecodeTuples to recode the values of the tuples in int
           \return false if end of file  
        */
        template<class ContainerTuple >
        bool readTuple( ContainerTuple & tuple , RecodeToInt<string> * inRecodeTuples  );
              
        //! Function reading the attributes in the file.
        /**
           \param attrib stores the attributes (must have a push_back function).   
           \return false if end of file  
        */
        template<class ContainerAttrib > 
        bool readAttrib( ContainerAttrib & attrib  ) ;
              
        //! Function reading data in a file and store it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \param inRecodeTuples to recode the values of the tuples in int           
           \return true if data has been read.
        */
        template< class ContainerBdT >
        bool read( ContainerBdT & container  , RecodeToInt<string> * inRecodeTuples=0  );
        
        //! Function write data in a file.
        /**
           \param container to read and write in the output.      
           \param recode functor used to recode the data read and store the mapping.
           \param supp the support of the itemset.
        */
        template< class Iterator, class Val >
        void write(Iterator begin, Iterator end, Val &  supp ) ;
        
        //! Function write data in a file.
        /**
           \param container to read and write in the output.      
           \param recode functor used to recode the data read and store the mapping.
        */
        template< class Iterator >
        void write(Iterator begin, Iterator end ) ;
        
        //! Functore used to save an element of a solution in the file.
        class saveElemInFile
        {
            ofstream * f ;
            
            public:
                 
                 saveElemInFile( ofstream * inFile ) { f = inFile ; }                
                                   
                 template< class T >     
                 void operator() (T & x) { (*f)<< x<<" ";}
        };
        
        //! Functor used to save a solution in the file.
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
        char * fileName;
        
        //! Stream to read in the file
        ifstream readfile;
        
        //! Stream to write in the file
        ofstream writefile;      
        
        //! Recode the value of the tuples in integer
        RecodeToInt<string>  * recodeTuples ; 

                  
    public:
        
        //! List of all the attributes.
        vector<string> attributes;
        
        //! Constructor.
        /**
           The constructor open the file and keep it open until the destruction of the object.
           
           \param inFileName name of the file to access
           \param inrecodeValAttrib recode function to recode values of the tuples in int
        */
        TabularDBFile( char * inFileName, RecodeToInt<string>  * inrecodeTuples=0 )
        {
           fileName= inFileName;
           free_recode=false;
           
           if( inrecodeTuples )
               recodeTuples = inrecodeTuples ;           
           else
           {
               recodeTuples = new RecodeToInt<string>;
               free_recode = true;
           }
           
                    
        }   
        
        //! Destructor.
        /**
           The destructor close the file.
        */  
        virtual ~TabularDBFile(){  if (!readfile.is_open() ) readfile.close(); if (!writefile.is_open() ) writefile.close(); if(free_recode && recodeTuples) delete recodeTuples; }
               
        //! Extract data of the file and insert it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \return *this.
        */   
        template< class ContainerBdR >
        TabularDBFile& operator>> ( ContainerBdR & container )
        { 
            if (!readfile.is_open() )
            {
                    readfile.open( fileName);       // open the file 
                   
                    readAttrib( attributes ) ; // read the attributes;
            }    
            
           
            while( !readfile.eof() ) read( container, recodeTuples) ; 
            return *this; 
        }      
         
        //! Extract data of the container (set of itemsets) and insert it in the file.
        /**
           \param container where the data is read (using iterators) and inserted in the file.     
           \return *this.
        */  
        template< class ContainerSet >
        TabularDBFile& operator<< ( ContainerSet & container )
        { 
            if (!writefile.is_open() )
                    writefile.open( fileName);       // open the file 
            
            saveContainerInFile save( &writefile );
            for( typename ContainerSet::iterator it = container.begin(); it != container.end();++it )
            {
                save( *it );
                
                if( typeid(it.measure()) != typeid(Boolean ))
                    writefile <<" :"<<it.measure();
                writefile <<"\n" ;
                
            }    
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
            
            write( setElement.begin(), setElement.end(), supp ) ;            
        }
        
        //! Insert an itemset in the file.
        /**
            This function corresponds to the classic push_back( ) function od STL container.
            The template parameter represents a container of integers.
            \param setElement the set of element to insert in the file.
        */             
        template< class Container >
        void push_back(  Container & setElement  )
        { 
            if (!writefile.is_open() )
                    writefile.open( fileName);       // open the file 
            
            write( setElement.begin(), setElement.end() ) ;            
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
                 
            write( first, last, supp ) ;
        }
        
        //! Insert a set of element in the file.
        /**
            The template parameter represents the input iterators on the integers to insert in the file.
            \param first iterator on the first element. 
            \param last iterator on the element after the last. 
        */             
        template< class InputIterator,  class Val>
        void push_back( InputIterator  first, InputIterator last  )
        {  
            if (!writefile.is_open() )
                    writefile.open( fileName);       // open the file 
                 
            write( first, last ) ;
        }
                         
};

//! Function reading a tuple in the file.
/**
   \param tuple stores the tuple (must have a push_back function).   
   \return false if end of file  
*/
template<class ContainerTuple > 
bool TabularDBFile::readTuple( ContainerTuple & tuple, RecodeToInt<string> * inRecodeTuples    )
{  
    
    if (!readfile.is_open() )
            readfile.open( fileName);       // open the file 
    
    if( readfile.is_open() && !readfile.eof()   )
    {  

        string buffer, buffer2;
        
        if( getline( readfile, buffer ) )// read one line in the file
        {
            string::size_type oldpos=0;   
            string::size_type pos=0;   

            int cptAttrib= 0 ; // number of the attributes 
            while(  (pos = buffer.find(";", oldpos ) ) && string::npos != pos && buffer!="\n" )
            {            
                tuple.push_back( (*inRecodeTuples)(  buffer.substr( oldpos, pos-oldpos ) ) );
    
                oldpos =pos+1;
                ++cptAttrib;             
            }
            

            tuple.push_back( (*inRecodeTuples)(  buffer.substr( oldpos, buffer.size()-oldpos ) ) );                                    
                
            return true;              
        }    
    } 
    
    return false;   
}    

//! Function reading the attributes in the file.
/**
   \param attrib stores the attributes (must have a push_back function).   
   \return false if end of file  
*/
template<class ContainerAttrib > 
bool TabularDBFile::readAttrib( ContainerAttrib & attrib  )
{  
    
    if (!readfile.is_open() )
            readfile.open( fileName);       // open the file 
    
    if( readfile.is_open() && !readfile.eof()   )
    {  

        string buffer;
        string::size_type oldpos=0;   
        string::size_type pos=0;   
        
        getline( readfile, buffer ) ;      

        while( (pos = buffer.find(";", oldpos ) ) && string::npos != pos )
        {
            attrib.push_back( buffer.substr( oldpos, pos-oldpos ) );

            oldpos =pos+1;
        }
            
        attrib.push_back( buffer.substr( oldpos, buffer.size()-oldpos ) );                                    

        return true;              
    } 
    
    return false;   
}    

//! Function reading data in a file and store it in a container.
/**
   \param container where the data read is inserted (must have a push_back function).     
   \return  false if end of file 
*/
template< class ContainerBdT >
bool TabularDBFile::read( ContainerBdT & container  , RecodeToInt<string>  * inRecodeTuples )
{
    if (!readfile.is_open() )
        readfile.open( fileName);       // open the file 

    vector<int> tuple; 
    bool data = readTuple( tuple, inRecodeTuples ) ;
    
    if( data  )
    {	      
            container.push_back( tuple );                         
    }
    return data;   
}    

//! Function write data in a file.
/**
   \param container to read and write in the output.      
   \param recode functor used to recode the data read and store the mapping
   \param supp the support of the itemset.
*/
template< class Iterator, class Val  >
void TabularDBFile::write( Iterator begin, Iterator end, Val & supp ) 
{
     saveElemInFile save( &writefile ) ;
     for_each( begin, end, save ) ; // save the itemset
     
     if( typeid(supp) != typeid(Boolean ))
          writefile<<" :"<<supp;
     
     writefile<<"\n";

}

//! Function write data in a file.
/**
   \param container to read and write in the output.      
   \param recode functor used to recode the data read and store the mapping
*/
template< class Iterator>
void TabularDBFile::write( Iterator begin, Iterator end ) 
{
     saveElemInFile save( &writefile ) ;
     for_each( begin, end, save ) ; // save the itemset
             
     writefile<<"\n";
}

#endif
