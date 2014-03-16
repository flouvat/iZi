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
 
#ifndef RECODETOINT_HXX
#define RECODETOINT_HXX

#include "Recode.hxx"

#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;


class printContainer;

//! Class RecodeToInt recode data oftype T in integers and stor the mapping.
/**
   The template parameter is the original type of the elements to recode in int.
   The internal id of the element are defined wrt the order they are passed to the functor.
   For example the first element passed to the functor have the id 0, the second element 1,...  
*/
template<class DataType>
class RecodeToInt: public Recode<DataType,int>
{ 
        typedef typename map<DataType,int>::iterator  mapDTintIt;  
        typedef typename list<DataType>::iterator  listDTIt;  
        typedef typename vector<DataType>::iterator  vectDTIt;
        
        //! Stores the solution of the recoding in internal id
        vector<int>  resultToIntid;
        
        //! Stores the solution of the remapping in the origial element
        vector<DataType>  resultToId;
        
        //! Functor that recodes an element if necessary and save the internal id in result. 
        class rec
        {
            RecodeToInt *recode;
            
            vector<int> * res ;
            
            public:
                                   
                rec( RecodeToInt * inRecode, vector<int> *inRes ){ recode = inRecode ; res = inRes ; }
                
                void operator () ( DataType & elem )
                {
                    mapDTintIt itmap ;
                    int size ;

                    itmap = recode->idTointernid.find( elem ) ;
                    if( itmap == recode->idTointernid.end() )         
                    {               
                       size =  recode->idTointernid.size() ;

                       // set the id to internal id attribute
                       recode->idTointernid[ elem ]=  size ;  

                       // we update the intidToid vector
                      
                      recode->intIdToid.push_back( elem ) ;                
                      
                      // insert in the result the new id               
                      res->push_back( size ) ; 
                    }
                    else
                       res->push_back(  itmap->second ) ;  

                }                            
        };
        
        //! Functor used to init an object with a set of element to recode        
        class recInit
        {
            RecodeToInt *recode;
            
            public:
                
                recInit( RecodeToInt * inRecode ){ recode = inRecode;  }
                
                void operator () ( DataType & elem )
                {              
                      // set the id to internal id attribute
                      // begin at  0
                      recode->idTointernid[ elem ]=  recode->idTointernid.size()-1 ;  
                                                     
                       // we update the intidToid vector                      
                      recode->intIdToid.push_back( elem ) ;                                                                             
                }                            
        };
        
        //! Functor used to remap an internal id to the original id
        class remapping
        {
            RecodeToInt *recode;
            
            vector<DataType> * res ;
            
            public:
                
                remapping(  RecodeToInt * inRecode  , vector<DataType> *inRes ){ recode = inRecode ; res = inRes ; }
                
                void operator () ( int & elem )
                {   
                      // remap to the original id
                      res->push_back( recode->intIdToid[ elem ] ) ;                                                                                                 
                }                        
        };
        
               
// protected:
 public:

       //! vector used to map the internal id to the real elemnt.
       vector<DataType>  intIdToid;

 public:
      
      //! Contructor
      RecodeToInt():Recode<DataType,int>(){}
      
      //! Contructor used to construct the recode object initialized with a SET (not a multi set) of DataType
      /**
         Since recode is empty, the sets are new sets to recode, there is no need to search if they already recoded.
      */
      template< class Container >
      RecodeToInt( Container & itemset  ):Recode<DataType,int>()
      { 
            intIdToid.reserve( itemset.size() );
                               
            // each new val  is recoded in an int (from 0)          
            for_each( itemset.begin(), itemset.end(), recInit(this) );
      }
            
      //! Destructor
      virtual ~RecodeToInt(){ } 
      
      //! Copy contructor.
      RecodeToInt( const RecodeToInt & rec ){ intIdToid = rec.intIdToid; (this->idTointernid) = rec.idTointernid;  }
      
      //! Affectation operator.
      RecodeToInt & operator = ( const RecodeToInt & rec )
      {                                    
                  intIdToid = rec.intIdToid; (this->idTointernid) = rec.idTointernid; 
				  return *this;
      }

      
      //! Map elements of the input data in internals ids.
      /**
         \param itemset set of elements to recode in int.
         \return the new ids.
      */
      template< class Container >
      vector<int> & operator() ( Container & itemset ); 
      
      //! Map elements of the input data in internals ids.
      /**
         \param first iterator on the first element
         \param last iterator on the element after the last
         \return the new ids.
      */             
      template< class InputIterator> 
      vector<int> & operator() ( InputIterator  first, InputIterator last ); 
      
      //! Map an element of the input data in an internals id.
      /**
         \return the new id.
      */
      virtual int operator() ( DataType item );      
      
      //! Remap internals ids in elements of the input data .
      /**
         \param itemset set of elements to recode in input Data type.
         \return the original ids .
      */
      template< class Container >
      vector<DataType> & remap( Container & itemset )
      {     
            resultToId.resize( 0 ); // set the size to 0 but doesn't deallocate the memory
            resultToId.reserve( itemset.size() ); 
                   
            for_each( itemset.begin(), itemset.end(), remapping( this, &resultToId ) );   
            
            return resultToId;               
      } 
      
      //! Remap internals set of ids in the corresponfing set of elements of the input data .
      /**
         \param itemset set of elements to recode in input Data type.
         \param remapItemsets for the original ids .
      */
      void  remap( vector<int> * itemset, vector<DataType> * remapItemsets )
      {

            if( remapItemsets->size() )
                remapItemsets->resize( 0 ) ; // sets the size to 0 but the memory is not deallocated

            for_each( itemset->begin(), itemset->end(), remapping( this, remapItemsets ) );  

      } 
    
      //! Remap internals ids in elements of the input data .
      /**
         \param first iterator on the first element to remap.
         \param last iterator on the element after the last to remap.
         \return the original ids.
      */             
      template< class InputIterator> 
      vector<DataType> & remap( InputIterator first, InputIterator  last )
      {
            resultToId.resize( 0 ); // set the size to 0 but doesn't deallocate the memory
                       
            for_each( first, last, remapping( this, &resultToId ) );   
            
            return resultToId;                           
      }      
      
      //! Map an element of the input data in an internals id.
      /**
         \return the new id.
      */
      virtual DataType remap( int  item )
      {
            return intIdToid[ item ];               
      }      
   
      
          
};


//! Map elements of the input data in internals ids.
/**
   \param itemset set of elements to recode in int.
   \return the new ids (ordered).
*/
template<class DataType> template< class Container >
vector<int> & RecodeToInt<DataType>::operator() ( Container & itemset )
{         
      resultToIntid.resize( 0 ) ;
      resultToIntid.reserve( itemset.size() ) ;    

      // each new val  is recoded in an int (from 0)          
      for_each( itemset.begin(), itemset.end(), rec(this, &resultToIntid) );      

      return resultToIntid;        
}

//! Map elements of the input data in internals ids.
/**   
   \param first iterator on the first element
   \param last iterator on the element after the last
   \return the new ids (ordered).
*/             
template<class DataType> template< class InputIterator> 
vector<int> & RecodeToInt<DataType>::operator() ( InputIterator  first, InputIterator  last )
{
      resultToIntid.resize( 0 ) ;       
    
      // each new val  is recoded in an int (from 0)          
      for_each( first, last, rec(this, &resultToIntid) );
      
      return resultToIntid; 
        
}

//! Map an element of the input data in an internals id.
/**
 \return the new id.
*/
template<class DataType>
int  RecodeToInt<DataType>::operator() ( DataType  item )
{
      mapDTintIt itmap ;
      int size ;
      int newid ;
      
      // the new val  is recoded (if necesary) in an int (from 0)    
      
      itmap = this->idTointernid.find( item ) ;
      
      if( itmap == this->idTointernid.end() )     //  the data has already not been recoded
      {
          
          size = this->idTointernid.size() ;
               
          // set the id to internal id attribute
          this->idTointernid[ item ]=  size ;  
                          
          newid = size ; 
          
          // we update the intidToid vector
          
          intIdToid.push_back( item ) ;
      }
      else 
          newid = itmap->second ;              
      
      return newid ;
      
}



//! Functor used to print to screen a container
class printContainer
{
      char end ;
      
   public:
       printContainer(char inend ='\n' ){ end = inend; }

  template< class T>  
  void operator() (T x) { cout<<x<<end;}
  
  void operator() (string x) {cout<<x.c_str() <<end;}
};    

//! Functor used to print to screen a container of container  
class printContainerOfContainer 
{
   public:
       printContainerOfContainer( ){}

  template< class T1 >     
  void operator() (T1 x) { for_each( x.begin(), x.end(), printContainer(' ')  ); cout<<endl;    }
};   

//! Functor used to print to screen a container of iterator on elements
template< class Type > 
class printContainerOfIterator 
{         
   public:
       printContainerOfIterator(){}

  template< class T1 >     
  void operator() (T1 x) { for_each( x->begin(), x->end(), printContainer(' ')  ); cout<<endl;    }
};  





//! Functor used to save the data of a container in a file
class saveContainer
{
      //! name of the file to used
      char * fileName;
      
      //! character inserted after each element of the container        
      char end ;
      
      //! Stream used to write in the file
      ofstream * writefile;     
      
   public:
          
      saveContainer(char * inFileName, char inend ='\n' )
      {                        
         fileName = inFileName;
         
         writefile = new ofstream( fileName);       // open the file 
        
         end = inend; 
      }

      saveContainer( ofstream * inWriteFile, char inend ='\n' ){ fileName= 0; writefile =  inWriteFile;  end = inend; }

      ~saveContainer()
      { 
          if( fileName  )
            writefile->close(); 
      }

      
      template< class T>  
      void operator() (T x) { *writefile <<x<<end;}
      
      void operator() (string x) { *writefile <<x.c_str() <<end;}
};    


//! Functor used  to save the data of a container of container  
class saveContainerOfContainer 
{
      //! name of the file to used
      char * fileName;
           
      //! Stream used to write in the file
      ofstream writefile;     
      
   public:
          
      saveContainerOfContainer(char * inFileName )
      { 
         fileName = inFileName;
         
         if (!writefile.is_open() )   
            writefile.open( fileName);       // open the file 
         
      }
      
      template< class T1 >     
      void operator() (T1 x) { for_each( x.begin(), x.end(), saveContainer(&writefile,' ')  ); writefile<<endl;    }
};   

#endif
