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
// Example of main program for the discovery of the theory or the borders of the 
// frequent essential itemsets using algorithms Apriori
//******************************************************************************


int main(int argc, char *argv[])
{



//******************************************************************************
// Define the name and format of the file containing the transactional database 
//******************************************************************************

// the file is in the FIMI format
FimiFile<>  fimi03("connect.dat" ); // "connect.dat" is the name of the file containing the data


//******************************************************************************
// Define a structure to store the db in memory
//******************************************************************************

// the data will be store in a trie (TaTRee)
//      int parameter : the transaction are composed of integers values
typedef BinaryDB< int, Tatree_base<int> > TransDB;

// Initialize the object: read and copy the data
TransDB db( fimi03 ) ;


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
predEss  isfreqess( db, 40000 ); //initialize data and minimum support threshold


//******************************************************************************
// Define the initialization function for the algorithm 
//******************************************************************************

// The functor will search the attributes of the relation in the file.
// These attributes will be used to initialize the exploration of the algorithm
InitFrequent< int, TransDB, predEss >  init( db, isfreqess );


//******************************************************************************
// Define the outputs
//******************************************************************************

// define two files to save the borders
FimiFile<> bdneg("BdN.dat");
FimiFile<> bdpos("BdP.dat");

// Use the class RecordDistribution to save the distribution of Bd+
// (could be use for Bd- too)
RecordDistribution< FimiFile<> >  distribbdN( &bdneg ); 
RecordDistribution< FimiFile<> >  distribbdP( &bdpos ); 


//******************************************************************************
// Use an algorithm
//******************************************************************************

// Declare algorith Apriori with 
//         int: the type of the itemsets
Apriori<int,SupportDisj > apriori;

// To print screen informations about the exectution
apriori.verbose = true ;

// exectue Apriori algorithm to find the boders
apriori( init, isfreqess, none, &distribbdP, &distribbdN);
 

// print to screen the distribution of Bd- 
cout<<"Distrib bd+"<<endl;
cout<<distribbdP<<endl;


cout<<"Distrib bd-"<<endl;
cout<<distribbdN<<endl;
 


return 0;
}

