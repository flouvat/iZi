#include <iostream>
#include <time.h>

using namespace std; 

#include "TabularDBFile.hxx"
#include "FdepFile.hxx"

#include "Support.hpp"

#include "Key_base.hxx"
#include "Not_Predicate.hxx"

#include "InitKey.hxx"

#include "RecordDistribution.hxx"


#include "Apriori.hxx"
#include "StopIteDistrib.hxx"
#include "Abs.hxx"
#include "AprioriReverse.hxx"
#include "AbsReverse.hxx"

//******************************************************************************
// Example of main program for the discovery of the borders of the 
// the super key in a relation using algorithms Apriori or ABS
//******************************************************************************

int main(int argc, char *argv[])
{
    
if(argc < 3) {
    cerr << "usage: " << argv[0] << " datafile " << endl;
    cerr << "\t -apriori          : use apriori algorithm (default)"<< endl;    
    cerr << "\t -abs k            : use abs algorithm and the first dualization is at level \"k\" "<< endl;        
    cerr << "\t -reverse          : use a reverse algorithm"<< endl;        
    cerr << "\t -notpred          : use the inverse predicate"<< endl;    
    cerr << "\t -v                : print to screen"<< endl;
    cerr << "\t -th file_name     : save the theory in \"file_name\" "<< endl;
    cerr << "\t -bdp file_name    : save the positive border in \"file_name\" "<< endl;
    cerr << "\t -bdn file_name    : save the negative border in \"file_name\" "<< endl;    
}
else 
{

    bool verbose = false;
    bool use_apriori = true ;
    bool notpred = false ;
    bool reverse = false ;
    int levl = 2 ; // level of the first dualization for abs
    char * bdpfile = 0 ;
    char * bdnfile = 0 ;
    char * thfile = 0 ;
    
    int i = 2; 
    while( i < argc )
    {
             
          if( strcmp( argv[ i ], "-v" ) == 0 || strcmp( argv[ i ], "v" ) == 0 )
          {
               verbose = true ;
          }
      	  else if( strcmp( argv[ i ], "-bdp") == 0 )
      	  {
               i++ ;
          	   bdpfile = argv[ i ];
          }	   
      	  else if( strcmp( argv[ i ], "-bdn") == 0 )
          {
               i++ ;
           	   bdnfile = argv[ i ];
          }
      	  else if( strcmp( argv[ i ], "-th") == 0 )
          {
               i++ ;
          	   thfile = argv[ i ];
          }    
          else if( strcmp( argv[ i ], "-abs") == 0 )
          {
               i++ ;
               use_apriori =false ;   
               levl = atoi(argv[ i ]);
  
          }
          else if( strcmp( argv[ i ], "-reverse") == 0 )
          {
               reverse =true ;                          
           }  
          else if( strcmp( argv[ i ], "-notpred") == 0 )
          {
               notpred =true ;                          
           }                     	               	            
                     
          i++ ;             
    }

   
    //******************************************************************************
    // Define the name and format of the file containing the relational database 
    //******************************************************************************
    
    // if the relation is stored in a csv format:
    typedef FdepFile RelationFile; 
    
    RelationFile dbr( argv[1] ); // "bda1.csv" is the name of the file containing the relation
        
    ///******************************************************************************
    // Copy the tuples of the relation in a data structure in memory 
    //******************************************************************************
    
    // read the relation and copy the data in memory
    vector< vector<int> > relation;
    dbr >> relation;
       
    //******************************************************************************
    // Define the predicate 
    //******************************************************************************
    
    // Use the following class to use a basic implementation of the predicate "being a super key"
     typedef Key_base< vector< vector<int> >, string  > KeyPred;
     
    // initialize the predicate
    KeyPred key(  relation, dbr );
    
    // invert the predicate
    // if we want to do a bottom up exploration of the search space
    // (since the predicate is monotone)
    Not_Predicate< KeyPred > notkey( key ) ; 


    //******************************************************************************
    // Define the initialization function for the algorithm 
    //******************************************************************************
    
    // The functor will search the attributes of the relation in the file.
    // These attributes will be used to initialize the exploration of the algorithm
    InitKey<string,RelationFile > init( dbr );

    
   
    //******************************************************************************
    // Define the outputs
    //******************************************************************************
    
    TabularDBFile * bdneg = 0 ;
    TabularDBFile * bdpos = 0 ;
    TabularDBFile * th=  0;
    
    // define two files to save the borders
    if( bdnfile )
        bdneg = new TabularDBFile(bdnfile);
        
    if( bdpfile )    
        bdpos= new TabularDBFile(bdpfile);

    if( thfile )    
        th= new TabularDBFile(thfile);
     
        
    // Use the class RecordDistribution to save the distribution of Bd+
    // (could be use for Bd- too)
    RecordDistribution< TabularDBFile >  distribbdN( bdneg ); 
    RecordDistribution< TabularDBFile >  distribbdP( bdpos ); 
    RecordDistribution< TabularDBFile >  distribth( th ); 
    
    
    //******************************************************************************
    // Use an algorithm
    //******************************************************************************
    
    
    if( use_apriori )
    {

        if ( !reverse )        
        {
            // Declare algorith Apriori with 
            //         string: the type of the attributes
            Apriori<string> apriori;
            
            // To print screen informations about the exectution
            apriori.verbose = verbose ;
            
            if( notpred )
                    // Use Apriori algorithm to find the boders doing a  bottom up exploration
                    //         string: the type of the attributes
                    apriori( init, notkey, &distribth, &distribbdP, &distribbdN);
            else
                    apriori( init, key, &distribth, &distribbdP, &distribbdN);
        }
        else
        {
            // Use Apriori algorithm to find the boders doing a  top down exploration
            //         string: the type of the attributes
            AprioriReverse<string > apriorirev;
            apriorirev.verbose = verbose ; // To print screen informations about the exectution
            if( notpred )            
                apriorirev( init, notkey, &distribth, &distribbdP, &distribbdN);
            else
                apriorirev( init, key, &distribth, &distribbdP, &distribbdN);

            
        }
    }
    else 
    {

        StopIteDistrib stopApriori(levl) ;  // to stop the levelwise exploration at the levl-th iteration
            
        if ( !reverse )        
        {
            // To use ABS doing  a bottom up exploration
            Abs<string> abs;
            abs.verbose = verbose ;
            
            if( notpred )
                abs( init, notkey, &stopApriori,&distribbdP, &distribbdN);
            else
                abs( init, key, &stopApriori, &distribbdP, &distribbdN);
        }
        else
        {
            // To use ABS doing  a top down exploration
            AbsReverse<string> absrev;
            absrev.verbose = verbose ;
            
            if( notpred )
                absrev( init, notkey, &stopApriori, &distribbdP, &distribbdN);
            else
                absrev( init, key, &stopApriori, &distribbdP, &distribbdN);
                

        }
        
    } 
    
    // print to screen the distribution of Bd- 
    cout<<"Theory distribution"<<endl;
    cout<<distribth<<endl;
    
    cout<<"Bd+ distribution"<<endl;
    cout<<distribbdP<<endl;
    
    
    cout<<"Bd- distribution"<<endl;
    cout<<distribbdN<<endl;
    
    if( bdpos)  delete bdpos;
    if( bdneg)  delete bdneg;
    if( th )    delete th;
}


return 0;
}

