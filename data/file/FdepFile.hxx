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
 
#ifndef FDEPFILE_HXX
#define FDEPFILE_HXX

#include <fstream>
#include <string>

#include "RecodeToInt.hxx"


//! Read a relation saved in the Fdep format.
/**    
    The file format is:
        * first line: the relation schema
            relation( rel-head, [ attr-1, attr-2, ..., attr-n ]), 
                      with rel-head the name of the relation,
                      attr-i th name of the ith attribute.
        * each other line: one tuple    
            rel-head( value-of-attr-1, ..., value-of-attr-n ).
            
    see for more details: http://www.cs.bris.ac.uk/~flach/fdep/
*/
class FdepFile 
{
      
        //! Function reading a tuple in the file.
        /**
           \param tuple stores the tuple (must have a push_back function).
           \param recodeValAttrib use to recode attributes in integer.                  
           \return false if end of file  
        */
        template<class ContainerTuple >
        bool readTuple( ContainerTuple & tuple ,vector< RecodeToInt<string> > * recodeValAttrib  );
              
        //! Function reading the attributes in the file.
        /**
           \param attrib container storing the attributes (must have a push_back function).   
           \return false if end of file  
        */
        template<class ContainerAttrib > 
        bool readAttrib( ContainerAttrib & attrib  ) ;
              
        //! Function reading all the data in the file and store it in a container.
        /**
           \param container class representing the relation (must have a push_back function). 
           \param recodeValAttrib use to recode attributes in integer.    
           \return true if data has been read.
        */
        template< class ContainerDB >
        bool read( ContainerDB & container, vector< RecodeToInt<string> > * recodeValAttrib=0  );
        
   protected:
                  
         //! name of the file to used
        char * fileName;
        
        //! Stream used to read in the file
        ifstream readfile;
        
        //! Stream used to write in the file
        ofstream writefile;      
                  
    public:
        
        //! List of all the attributes.
        vector<string> attributes;
        
        //! Constructor.
        FdepFile( char * inFileName){ fileName= inFileName; };                                 
        
        //! Destructor.
        /**
           The destructor close the file.
        */  
        virtual ~FdepFile(){  if (!readfile.is_open() ) readfile.close(); if (!writefile.is_open() ) writefile.close(); }
               
        //! Extract data of the file and insert it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \return *this.
        */   
        template< class ContainerDB >
        FdepFile& operator>> ( ContainerDB & container )
        { 
            if (!readfile.is_open() )
            {
                    readfile.open( fileName);       // open the file 
                   
                    readAttrib( attributes ) ; // read the attributes;
            }    
            

            vector< RecodeToInt<string> > recodeValAttrib( attributes.size() ) ; // recode the value of the attributes in integer;
           
            while( !readfile.eof() ) read( container, &recodeValAttrib) ; 
            return *this; 
        }      
         

};

//! Function reading a tuple in the file.
/**
   \param tuple stores the tuple (must have a push_back function).   
   \param recodeValAttrib use to recode attributes in integer.   
   \return false if end of file  
*/
template<class ContainerTuple > 
bool FdepFile::readTuple( ContainerTuple & tuple, vector< RecodeToInt<string> > * recodeValAttrib    )
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
            oldpos= buffer.find("(");   
            pos=0;   
            int cptAttrib= 0 ; // number of the attributes 

            if(  oldpos!=  string::npos )
            {        
               oldpos =oldpos +1;
                 
                while(  (pos = buffer.find(",", oldpos ) )!=  string::npos && buffer!="\n" )
                {            
                    tuple.push_back( (recodeValAttrib->at(cptAttrib))(  buffer.substr( oldpos, pos-oldpos ) ) );
   
                    oldpos =pos+1;
                    ++cptAttrib;             
                }
            }
            
            if( cptAttrib )
            {
                pos = buffer.find(")")  ;  
                tuple.push_back( (recodeValAttrib->at(cptAttrib))(  buffer.substr( oldpos, pos-oldpos ) ) );
                return true;                                    
            }

                         
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
bool FdepFile::readAttrib( ContainerAttrib & attrib  )
{  
  
    if (!readfile.is_open() )
            readfile.open( fileName);       // open the file 

    if( readfile.is_open() && !readfile.eof()   )
    {  

        string buffer;
        string::size_type oldpos=0;   
        string::size_type pos=0;   
        
        getline( readfile, buffer,'\n' ) ;     

        if(  (oldpos = buffer.find("[")) !=  string::npos )
        { 
            oldpos =oldpos +1; 
            while( (pos = buffer.find(",", oldpos ) ) != string::npos )
            {
                attrib.push_back( buffer.substr( oldpos, pos-oldpos ) );
    
                oldpos =pos+1;
            }
        }

        pos = buffer.find("]") ;    
        attrib.push_back( buffer.substr( oldpos, pos-oldpos ) );                                    


        return true;              
    } 
    
    return false;   
}    

//! Function reading data in a file and store it in a container.
/**
   \param container where the data read is inserted (must have a push_back function).     
   \param recodeValAttrib use to recode attributes in integer.      
   \return  false if end of file 
*/
template< class ContainerDB >
bool FdepFile::read( ContainerDB & container  ,vector< RecodeToInt<string> > * recodeValAttrib )
{
    if (!readfile.is_open() )
        readfile.open( fileName);       // open the file 

    vector<int> tuple; 
    bool data = readTuple( tuple, recodeValAttrib ) ;
    
    if( data  )
    {	            
            container.push_back( tuple );                         
    }
    return data;   
}    


#endif
