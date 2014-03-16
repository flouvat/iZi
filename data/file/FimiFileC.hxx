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
 
#ifndef FIMIFILEC_HXX
#define FIMIFILEC_HXX

#include "ItFimiFile.hxx"


//! Class used to read a transactionnal db in the FIMI format and save data in the FIMI format.
/** 
    This class used C function to read and write the data in the file. 
    The template parameter ContainerTrans is the type of container used to store the transactions.
*/
template<class ContainerTrans= vector<int> >
class FimiFileC 
{
    protected:
        
        //! Function reading a transaction in the file.
        bool readTrans( ContainerTrans & trans  );
              
        //! Function reading data in a file and store it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \return true if data has been read.
        */
        template< class ContainerBdT >
        bool read( ContainerBdT & container  );
        
        //! Function write data in a file.
        /**
           \param begin iterator on the first element of the container to save
           \param supp the support of the itemset.
        */
        template< class Iterator, class Val >
        void write(Iterator begin, Iterator end, Val  supp ) ;
        
        //! Function write data in a file when the value associated with the elements are integers.
        /**
           \param begin iterator on the first element of the container to save
           \param supp the support of the itemset.
        */
        template< class Iterator >
        void write(Iterator begin, Iterator end, int  supp ) ;
        
        //! Functor used to save in the file each element of a set
        class saveElemInFile
        {
            FILE * f ;
            
            public:
                 
                 saveElemInFile( FILE * inFile ) { f = inFile ; }                
                                   
                 template< class T >     
                 void operator() (T & x) { fprintf(f,"%d ", x  ) ;}
        };
        
        //! Functor used to save in the file each set
        class saveContainerInFile 
        {
            FILE * f;
            
            public:
                 
                 saveContainerInFile( FILE * inFile ){ f = inFile; } 
                                                           
                 template< class Container >     
                 void operator() ( Container & cont )
                 {
                      saveElemInFile save(f) ; 

                      for_each( cont.begin(), cont.end(), save  );                        
                 }
        };
        
    protected:
              
         //! Name of the file.
        char* fileName ; 
        
         //! Pointer on the file opened in the constructor.
        FILE * file ;              

                  
    public:
           
        //! Used to have the same syntax tha the STL.
        typedef ItFimiFile< ContainerTrans > iterator ;    
        
        //! Constructor.
        /**
           The constructor open the file and keep it open until the destruction of the object.
        */
        FimiFileC( char * inFileName = 0)
        {
           fileName= inFileName;     
           file =0 ;                      
        }   
        
        //! Destructor.
        /**
           The destructor close the file.
        */  
        virtual ~FimiFileC(){ if( file ) fclose(file);  }
               
        //! Extract all the  data of the file and insert it in a container.
        /**
           \param container where the data read is inserted (must have a push_back function).     
           \return *this.
        */   
        template< class ContainerBdT >
        FimiFileC& operator>> ( ContainerBdT & container )
        {
                  if( !file ) file = fopen( fileName, "r+");       // open the input file 
                  if (!file) cerr<<"cannot open file "<< fileName<<endl;                      
                  while( !feof(file) ) read( container) ; 
                  return *this; 
        }      
         
        //! Extract all the data of the container (set of itemsets) and insert it in the file.
        /**
           \param container where the data is read and inserted in the file.     
           \return *this.
        */  
        template< class ContainerBdT >
        FimiFileC& operator<< ( ContainerBdT & container )
        { 
            if( !file ) file = fopen( fileName, "w+");       // open the input file 
            if (!file) cerr<<"cannot open file "<< fileName<<endl;                      
                  
            saveContainerInFile save( file );
            for( typename ContainerBdT::iterator it = container.begin(); it != container.end();++it )
            {
                save( *it );
                fprintf( file,"(%d)", it.measure().supp  ) ;
                fputc('\n', file );                                          
            }    
            return *this;
        }
         
        //! Return an iterator on the first element stored. 
        ItFimiFile<ContainerTrans> begin() { return ItFimiFile<ContainerTrans>( this );  };
            
        //! Return an iterator on the element after the last element stored. 
        ItFimiFile<ContainerTrans> end() { return ItFimiFile<ContainerTrans>() ; } ;        
             
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
                if( !file ) file = fopen( fileName, "w+");       // open the input file 
                if (!file) cerr<<"cannot open file "<< fileName<<endl;                      

                write( setElement.begin(), setElement.end(), supp ) ;            
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
                if( !file ) file = fopen( fileName, "w+");       // open the input file 
                if (!file) cerr<<"cannot open file "<< fileName<<endl;                      

                write( first, last, supp ) ;
        }
                         
                   
        friend class ItFimiFile<ContainerTrans>;
        
};

//! Function reading a transaction in the file.
/**
   \param trans store the transaction (must have a push_back function).   
   \return false if end of file  
*/
template<class ContainerTrans > 
bool FimiFileC<ContainerTrans >::readTrans( ContainerTrans & trans  )
{
    
   if( !feof(file)   )
    {                                            
        char c;  
        int item=0, pos=0; 
        
        do
        {     
            item=0, pos=0;          
            c = getc(file);
                           
            while( ( c >= '0' ) && ( c <= '9' ) )
            {
                  item *=10;
                  item += int(c)-int('0');
                  c = getc(file);
                  pos++;
            }
            
            if(pos)
                  trans.push_back( item );                                    
             
        }while( c != '\n' && !feof( file ) );
        
        return true;  
            
    } 
    
    return false;   
}    

//! Function reading data in a file and store it in a container.
/**
   \param container where the data read is inserted (must have a push_back function).     
   \return  false if end of file 
*/
template<class ContainerTrans > template< class ContainerBdT >
bool FimiFileC<ContainerTrans >::read( ContainerBdT & container  )
{
   ContainerTrans trans; 
   bool data = readTrans( trans ) ;
   
   if( data  )
   {	      
            container.push_back( trans );                         
   }
   return data;   
}    

//! Function write data in a file.
/**
   \param container to read and write in the output.      
   \param recode functor used to recode the data read and store the mapping
   \param supp the support of the itemset.
*/
template<class ContainerTrans > template< class Iterator, class Val  >
void FimiFileC<ContainerTrans >::write( Iterator begin, Iterator end, Val supp ) 
{
     saveElemInFile save( file ) ;
     for_each( begin, end, save ) ; // save the itemset

     int s;
     s = supp.supp ;
     fprintf(file,"(%d)", s  ) ;
     fputc('\n', file );               		    
}
  
template<class ContainerTrans > template< class Iterator >
void FimiFileC<ContainerTrans >::write( Iterator begin, Iterator end, int supp ) 
{
     saveElemInFile save( file ) ;
     for_each( begin, end, save ) ; // save the itemset

     int s;
     s = supp ;
     fprintf(file,"(%d)", s  ) ;
     fputc('\n', file );               		    
}    

#endif
