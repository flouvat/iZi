#include <iostream>
#include <time.h>

using namespace std; 

#include "FimiFile.hxx"
#include "FimiFileC.hxx"

#include "BinaryDB.hxx"

#include "Essential.hxx"

#include "InitFrequent.hxx"

#include "PTree.hxx"
#include "RecordDistribution.hxx"

#include "Apriori.hxx"
#include "StopIteDistrib.hxx"
#include "Abs.hxx"

//******************************************************************************
// Example of main program for the discovery of the borders of the 
// frequent itemsets using algorithms Apriori or ABS
//******************************************************************************


int main(int argc, char *argv[])
{



if(argc < 3) {
    cerr << "usage: " << argv[0] << " datafile minsup(absolute)" << endl;
    cerr << "\t -v                : print to screen"<< endl;
    cerr << "\t -th file_name     : save the theory in \"file_name\" "<< endl;
    cerr << "\t -bdp file_name    : save the positive border in \"file_name\" "<< endl;
    cerr << "\t -bdn file_name    : save the negative border in \"file_name\" "<< endl;    
}
else 
{

    bool verbose = false;
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
                     
          i++ ;             
    }

   
    //******************************************************************************
    // Define the name and format of the file containing the transactional database 
    //******************************************************************************
    
    // the file is in the FIMI format
    FimiFile<>  fimi03( argv[1] ); // "connect.dat" is the name of the file containing the data
    
    
    //******************************************************************************
    // Define a structure to store the db in memory
    //******************************************************************************
    
    // the data will be store in a trie (TaTRee)
    //      int parameter : the transaction are composed of integers values
    typedef BinaryDB< int, Tatree_base<int> > TransDB;
    
    // Initialize the object: read and copy the data
    TransDB db( fimi03, verbose ) ;
    
    
    //******************************************************************************
    // Define the predicate 
    //******************************************************************************
    
    // Define the predicate "being frequent"
    // The db is in memory in a TransDB and the itemsets are sets of integers
    // This predicate is optimized for trie data structure 
    // and cannot be used when changing the exploration of the algorithm
    typedef Essential< TransDB, int> predEss;
    
    
    // initialize the predicate
    // the first parameter is the data and the second the minimum support threshol (absolute)
    predEss  isfreqess( db, atoi( argv[2] ) ); //initialize data and minimum support threshold
    isfreqess.verbose = verbose ;
    
    //******************************************************************************
    // Define the initialization function for the algorithm 
    //******************************************************************************
    
    // The functor will search the attributes of the relation in the file.
    // These attributes will be used to initialize the exploration of the algorithm
    InitFrequent< int, TransDB, predEss >  init( db, isfreqess );
    
    
    //******************************************************************************
    // Define the outputs
    //******************************************************************************
    
    FimiFile<> * bdneg = 0 ;
    FimiFile<> * bdpos = 0 ;
    FimiFile<> * th=  0;
    
    // define two files to save the borders
    if( bdnfile )
        bdneg = new FimiFile<>(bdnfile);
        
    if( bdpfile )    
        bdpos= new FimiFile<>(bdpfile);

    if( thfile )    
        th= new FimiFile<>(thfile);
     
        
    // Use the class RecordDistribution to save the distribution of Bd+
    // (could be use for Bd- too)
    RecordDistribution< FimiFile<> >  distribbdN( bdneg ); 
    RecordDistribution< FimiFile<> >  distribbdP( bdpos ); 
    RecordDistribution< FimiFile<> >  distribth( th ); 
    
    
    //******************************************************************************
    // Use an algorithm
    //******************************************************************************
    
    // Declare algorith Apriori with 
    //         int: the type of the itemsets
    Apriori<int,SupportDisj > apriori;
    
    // To print screen informations about the exectution
    apriori.verbose = verbose ;
    
    // exectue Apriori algorithm to find the boders
    apriori( init, isfreqess, &distribth, &distribbdP, &distribbdN);
     
    
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

