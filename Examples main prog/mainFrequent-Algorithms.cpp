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

//******************************************************************************
// Define the name and format of the file containing the transactional database 
//******************************************************************************

// the file is in the FIMI format
FimiFile<>  fimi03("mushroom.dat" ); // the name of the file containing the data


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
predFreq  isfreq( db, 2438  ); 


//******************************************************************************
// Define the initialization function for the algorithm 
//******************************************************************************

// The functor will search the attributes of the relation in the file.
// These attributes will be used to initialize the exploration of the algorithm
InitFrequent< int, TransDB, predFreq >  init( db, isfreq );


//******************************************************************************
// Define the outputs
//******************************************************************************

// save the positive border in memory in a trie data structure
// the "Support" parameter is used to store an integer value with th elements
// here this value is the support of the itemset
PTree<int, Support>  bdN;

// define two files to save the borders
FimiFile<> bdneg("BdN.dat");
FimiFile<> bdpos("BdP.dat");

// Use the class RecordDistribution to save the distribution of Bd+
// (could be use for Bd- too)
RecordDistribution< FimiFile<> >  distribbdP( &bdpos ); 


//******************************************************************************
// Use an algorithm
//******************************************************************************

// Use Apriori algorithm with 
//         int: the type of the itemsets
//         Support: to save the cardinality of the projection processed
Apriori<int,Support > apriori;
apriori.verbose = true ; // To print screen informations about the exectution
//apriori( init, isfreq, none, &distribbdP, &bdN); // exectue Apriori algorithm to find the boders
 

// To use ABS do:
Abs<int,Support> abs;
abs.verbose = true ;
StopIteDistrib stopApriori(2) ;  // to stop the levelwise exploration at the 2th iteration
abs( init, isfreq, &stopApriori, &distribbdP, &bdN);

// print to screen the distribution of Bd- 
cout<<"Distrib bd+"<<endl;
cout<<distribbdP<<endl;
 
// save Bd- in a file too
bdneg<< bdN;



return 0;
}

