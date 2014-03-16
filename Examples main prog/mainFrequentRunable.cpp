#include <iostream>
#include <time.h>

using namespace std; 

#include "FimiFile.hxx"

#include "BinaryDB.hxx"

#include "Frequent.hxx"

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
    cerr << "\t -abs              : use ABS algorithm"<< endl;
    cerr << "\t -v                : print to screen"<< endl;
    cerr << "\t -th file_name     : save the theory in \"file_name\" "<< endl;
    cerr << "\t -bdp file_name    : save the positive border in \"file_name\" "<< endl;
    cerr << "\t -bdn file_name    : save the negative border in \"file_name\" "<< endl;    
}
else 
{

    bool verbose = false;
    bool useAbs = false ;
    bool bdp = false;
    bool bdn = false ;
    bool th = false ;
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
               bdp = true ;
               i++ ;
               
               if( i < argc && (argv[ i ])[0] != '-')
          	       bdpfile = argv[ i ];
          }	   
      	  else if( strcmp( argv[ i ], "-bdn") == 0 )
          {
               bdn=true ;
               i++ ;
               if( i < argc && (argv[ i ])[0] != '-')
           	   bdnfile = argv[ i ];
          }
      	  else if( strcmp( argv[ i ], "-th") == 0 )
          {
               th=true ;
               i++ ;
               if( i < argc && (argv[ i ])[0] != '-')
          	   thfile = argv[ i ];
          }    
          else if( strcmp( argv[ i ], "-abs") == 0 )
               useAbs = true ;             	               	            
                     
          i++ ;             
    }
    
    //******************************************************************************
    // Define the name and format of the file containing the transactional database 
    //******************************************************************************
    
    // the file is in the FIMI format
    FimiFile<>  fimi03( argv[1] ); // the name of the file containing the data
    
    
    //******************************************************************************
    // Define a structure to store the db in memory
    //******************************************************************************
    
    // the data will be store in a trie (TaTRee)
    //      int parameter : the transaction are composed of integers values
    typedef BinaryDB< int, Tatree<int> > TransDB;
    
    // Initialize the object: read and copy the data
    TransDB db( fimi03 ) ;
    
    
    //******************************************************************************
    // Define the predicate 
    //******************************************************************************
    
    // Define the predicate "being frequent"
    // The db is in memory in a TransDB and the itemsets are sets of integers
    // This predicate is optimized for trie data structure 
    // and cannot be used when changing the exploration of the algorithm
    typedef Frequent< TransDB, int> predFreq;
    
    // initialize the predicate
    // the first parameter is the data and the second the minimum support threshol (absolute)
    predFreq  isfreq( db, atoi( argv[2] ) ); 
    
    
    //******************************************************************************
    // Define the initialization function for the algorithm 
    //******************************************************************************
    
    // The functor will search the attributes of the relation in the file.
    // These attributes will be used to initialize the exploration of the algorithm
    InitFrequent< int, TransDB, predFreq >  init( db, isfreq );
    
    
    //******************************************************************************
    // Define the outputs
    //******************************************************************************
    
    FimiFile<> * bdneg = 0 ;
    FimiFile<> * bdpos = 0 ;
    FimiFile<> * the=  0;
    
    // define two files to save the borders
    if( bdnfile )
        bdneg = new FimiFile<>(bdnfile);
        
    if( bdpfile )    
        bdpos= new FimiFile<>(bdpfile);

    if( thfile )    
        the= new FimiFile<>(thfile);
        
    // Use the class RecordDistribution to save the distribution of Bd+
    // (could be use for Bd- too)
    RecordDistribution< FimiFile<> > *  distribbdN = 0;
    RecordDistribution< FimiFile<> > * distribbdP= 0; 
    RecordDistribution< FimiFile<> >  * distribth = 0 ;
    
    
    if( bdn )
        distribbdN = new RecordDistribution< FimiFile<> >( bdneg ) ;
        
    if( bdp )
        distribbdP = new RecordDistribution< FimiFile<> >( bdpos ) ;
        
    if( th )
         distribth = new RecordDistribution< FimiFile<> >( the );
        
    //******************************************************************************
    // Use an algorithm
    //******************************************************************************    
    
    if( !useAbs )
    {
    
        // Use Apriori algorithm with 
        //         int: the type of the itemsets
        //         Support: to save the cardinality of the projection processed
        Apriori<int,Support > apriori;
        apriori.verbose = verbose ; // To print screen informations about the exectution
        apriori( init, isfreq, distribth, distribbdP, distribbdN); // exectue Apriori algorithm to find the boders
         
    }
    else
    {
        
        // To use ABS do:
        Abs<int,Support> abs;
        abs.verbose = verbose ;
        StopIteDistrib stopApriori(6) ;  // to stop the levelwise exploration at the 2th iteration
        abs( init, isfreq, &stopApriori, distribbdP, distribbdN);
    }
    
    // print to screen the distribution of Bd- 
    if( distribth)
    {
        cout<<"Theory distribution"<<endl;
        cout<<*distribth<<endl;
    }
    
    if( distribbdP )
    {    
        cout<<"Bd+ distribution"<<endl;
        cout<<*distribbdP<<endl;    
    }
    
    if( distribbdN )
    {    
         cout<<"Bd- distribution"<<endl;
         cout<<*distribbdN<<endl;
    }

    if( bdpos)  delete bdpos;
    if( bdneg)  delete bdneg;
    if( the )   delete the;
    
    if( distribbdN ) delete distribbdN;
    if( distribbdP ) delete distribbdP ;
    if( distribth ) delete distribth;
}

return 0;
}

