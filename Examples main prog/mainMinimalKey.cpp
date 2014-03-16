#include <iostream>
#include <time.h>

using namespace std; 

#include "TabularDBFile.hxx"
#include "FdepFile.hxx"

#include "Support.hpp"

#include "Key_base.hxx"
#include "Not_predicate.hxx"

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

//******************************************************************************
// Define the name and format of the file containing the relational database 
//******************************************************************************

// if the relation is stored in a csv format:
typedef TabularDBFile RelationFile; 

RelationFile dbr("bda1.csv"); // "bda1.csv" is the name of the file containing the relation


//******************************************************************************
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

// save the positive border in memory in a trie data structure
PTree<string>  bdP;

// define two files to save the borders
TabularDBFile bdneg("BdNbda1.dat");
TabularDBFile bdpos("BdPbda1.dat");

// Use the class RecordDistribution to save the distribution of Bd-
// (could be use for Bd+ too)
RecordDistribution< TabularDBFile >  distribbdN( &bdneg ); 


//******************************************************************************
// Use an algorithm
//******************************************************************************


// Use Apriori algorithm to find the boders doing a  bottom up exploration
//         string: the type of the attributes
Apriori<string > apriori;
apriori.verbose = true ; // To print screen informations about the exectution
//apriori( init, notkey, none, &bdP, &distribbdN);       


// Use Apriori algorithm to find the boders doing a  top down exploration
//         string: the type of the attributes
AprioriReverse<string > apriorirev;
apriorirev.verbose = true ; // To print screen informations about the exectution
//apriorirev( init, key, none, &bdP, &distribbdN); 


// To use ABS doing  a bottom up exploration
Abs<string> abs;
abs.verbose = true ;
StopIteDistrib stopApriori(2) ;  // to stop the levelwise exploration at the 2nd iteration
//abs( init, notkey, &stopApriori, &bdP, &distribbdN);     


// To use ABS doing  a top down exploration
AbsReverse<string> absrev;
absrev.verbose = true ;
absrev( init, key, &stopApriori, &bdP, &distribbdN); 


// print to screen the distribution of Bd- 
cout<<"Distrib bd-"<<endl;
cout<<distribbdN<<endl;
 
// save bd+ in a file too
bdpos<< bdP;



return 0;
}

