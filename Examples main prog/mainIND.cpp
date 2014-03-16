#include <iostream>
#include <time.h>

using namespace std; 

#include "TabularDBFile.hxx"

#include "Support.hpp"

#include "SatisfiedIND.hxx"
#include "Not_predicate.hxx"

#include "InitIND.hxx"

#include "RecordDistribution.hxx"

#include "Ens.hpp"

#include "Apriori.hxx"
#include "AprioriReverse.hxx"
#include "StopIteDistrib.hxx"
#include "Abs.hxx"
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

RecodeToInt< string > recodeValTuple;

RelationFile r1("bda1.csv", &recodeValTuple ); // "bda1.csv" is the name of the file containing the relation

RelationFile r2("bda2.csv", &recodeValTuple); // "bda1.csv" is the name of the file containing the relation


//******************************************************************************
// Copy the tuples of the relation in a data structure in memory 
//******************************************************************************

// read the relation and copy the data in memory
vector< vector<int> > relation1;
r1 >> relation1;

vector< vector<int> > relation2;
r2 >> relation2;


//******************************************************************************
// Define the predicate 
//******************************************************************************

// Use the following class to use a basic implementation of the predicate "being a satisfied IND"
typedef SatisfiedIND< vector< vector<int> >  > INDPred;
 
// initialize the predicate
INDPred isadi(  relation1, r1, relation2, r2 );

// invert the predicate
// if we want to do a bottom up exploration of the search space
// (since the predicate is monotone)
Not_Predicate< INDPred > notadi( isadi ) ; 


//******************************************************************************
// Define the initialization function for the algorithm 
//******************************************************************************

// The functor will search the attributes of the relation in the file.
// These attributes will be used to initialize the exploration of the algorithm
InitIND<RelationFile > init(r1,r2 );


//******************************************************************************
// Define the outputs
//******************************************************************************

// save the positive border in memory in a vector
vector< IND >  bdP;
vector< IND >  bdN;

// Use the class RecordDistribution to save the distribution of Bd- and Bd+
RecordDistribution<vector< IND > >  distribbdN( & bdN );
RecordDistribution<vector< IND > >  distribbdP( &bdP); 


//******************************************************************************
// Use an algorithm
//******************************************************************************

// function used to transform IND in sets
Ens ens;

// exectue Apriori algorithm to find the boders
// doing a bottom up exploration
Apriori<UnaryIND> apriori; // Declare algorith Apriori with 
apriori.verbose = true ;  // To print screen informations about the exectution
apriori( init, isadi, none, &distribbdP, &distribbdN, ens ); 


// exectue Apriori algorithm to find the boders
// doing a top down exploration
AprioriReverse<UnaryIND, Boolean, IND> apriorirev;
apriorirev.verbose = true ;
//apriorirev( init, notadi, none, &distribbdP, &distribbdN, ens );  
 

// To use ABS with top down exploration do:
Abs<UnaryIND> abs;
abs.verbose = true ;
StopIteDistrib stopApriori(2) ;  // to stop the levelwise exploration at the 2nd iteration
//abs( init, isadi, &stopApriori, &distribbdP, &distribbdN, ens ); 

// To use ABS with top down exploration do:
AbsReverse<UnaryIND, Boolean, IND> absrev;
absrev.verbose = true ;
//absrev( init, notadi, &stopApriori, &distribbdP, &distribbdN, ens);      // to do a bottom up exploration



// print to screen the distribution of Bd- 
cout<<"Distrib bd-"<<endl;
cout<<distribbdN<<endl;

// print to screen the distribution of Bd- 
cout<<"Distrib bd+"<<endl;
cout<<distribbdP<<endl;


// print bd+
cout<<"bd+"<<endl;
for_each( bdP.begin(), bdP.end(), printContainer() ) ;

// print bd-
cout<<"bd-"<<endl;
for_each( bdN.begin(), bdN.end(), printContainer() ) ;


// save bd+
cout<<"save bd+";
for_each( bdP.begin(), bdP.end(), saveContainer("BdPIND.dat") ) ;

// save bd-
cout<<"save bd-";
for_each( bdN.begin(), bdN.end(), saveContainer("BdNIND.dat") ) ;


return 0;
}

