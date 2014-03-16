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

#ifndef ABS_HXX
#define ABS_HXX

#include <set>

#include "Apriori.hxx"
#include "NoOutput.hxx"


//! Functor finding the negative border Bd- and/or the positive border Bd+ using the algorithm ABS.
/**
   The algorithme ("()" method)take in parameter:
        the initialisation functor, 
        the predicate,
        the functor determining at which iteration the levelwise step is stop,
        the positive and/or negative borders (optional),
        the transformation function (optional).
   
   The method executeAlgorithm could be use to execute the algorithm. 
   This method have another parameter to deal with "almost interesting elements" 
   based on an error function passed i n parameter.
   These elements are considered as interesting during the iterations, and at the end
   they are used to do a top-down traversal of the search space to find the latests elements of Bd+.
        
   The template parameter SetsType represents the type of the elements of the set representation.
   The template parameter Measure is the type of the value eventually associated with the word of the language.
   The template parameter Cand_DataStruct is the data structure type used by the algorithms to manipulate the candidates generated.
*/
template< class SetsType=int, class Measure=Boolean, class Cand_DataStruct = PTree<SetsType,Measure > >
class Abs
{         
         //! Class used when there is no error functor
        class NoError
        {
            public:           
                template< class Iterator>        
                bool operator() ( Iterator it ) {return false;}                   
        };
        
        // Iterator on the candidate dat structure
        typedef typename Cand_DataStruct::iterator ItCand; 
        
        // deque type used to store iterator on the candidates data structure
        typedef deque< ItCand > deqItCand ; 
        
        
        //! Method to get the init function the list of all the items (set representation)
        template< class f >
        class GetItems
        {
               
                // function transforming words in sets
                f * wordToSet;
                
                // store the items
                deque<SetsType> * listElem; 
                
                // when the problems studied is composed of itemset (transformation function = id)
                // items can directly be inserted in listElements
                template< class Word >
                void insItems( Word & w, Id * id)
                {       
                    listElem->push_back(  w  ) ; 
                } 
                
                // when the problems studied is composed of itemset 
                // but a transformation fucntion is used to
                // explore the search space top-down (transformation function = complement)
                // items can directly be inserted in listElements
                template< class Word >
                void insItems( Word & w, Complement<SetsType> * complem)
                { 
                     listElem->push_back(  w  ) ; 
                }  

                // when the problems studied is not composed of itemset 
                // but a transformation fucntion is used to
                // explore the search space top-down 
                // elements are transformed in sets and inserted in listElements
                template< class Word, class WordToSet >
                void insItems( Word & w, TransformElement< Complement<SetsType> ,SetsType, WordToSet , Word>  * wordToSetComplem)
                { 
                    listElem->push_back( *( (*wordToSetComplem->wordtoset)( & w )->begin() ) ) ;                                   
                }  
                                
                
                // when the problems studied is not composed of itemset 
                // elements are transformed in sets and inserted in listElements     
                template< class Word, class f2  >
                void insItems(  Word & w, f2 * inwordToSet)
                {  
                     vector<SetsType>* tmp =  (*inwordToSet)( &w );
                 
                     listElem->push_back( tmp->at(0)  ) ; // insert in candidates the set representaion of the word 
                }           
                                                                         
            public:
                GetItems(  f& inwordToSet, deque<SetsType> * inlistElem  ){ wordToSet = &inwordToSet; listElem  = inlistElem  ;  }
                
                template< class Word >
                void operator() (Word & w)
                { 
                     insItems( w, wordToSet);
                }                         
        }; 
             
        //! list of the interesting items of size 1 in the representation as sets
        deque<SetsType> listInterestItems;      
         
    protected:
        
        //! Execute the algorithm.
        /**
           \param init functor initializing the interesting items wrt predicate.
           \param pred the predicate 
           \param stopDualization functor determining when the apiori execution must stop 
           \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
           \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
           \param error function processing if a not interesting element of the optimistic positive border is "almost interesting"
        */
        template< class InitFunctor, class Predicate , class StopIteration, class OutputBdP, class OutputBdN, class Error, class f  >   
        void executeAlgorithm( InitFunctor & init, Predicate * pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN, Error * error, f & wordToSet );       	
 
        //! Process the optimist positive border
    	/**
           All the interesting sets update Bd+ and the subests of the not interesting sets are generated.
           If the "almost interesting" elements are studied, this method save these elements in a particular data structure.
           To optimize the minimal transversal computation, minimal transversals which have generated an interesting element are stored separately.
           
           \param pred the predicate 
           \param complSet set of complements of the minimal transversals in the current iteration
           \param opt set used to store the almost interesting elments (not necessarly used)
           \param tr set of the minimal transversals  find until the current iteration
           \param freqtr set of the minimal transversals  find until the current iteration and which have generated an interesting element
           \param subSet strore the subsets of size k (current level of the levelwise approach) of the not interesting elements fond in the optimistic positive border
           \param size current level of the levelwise approach
           \param wordToset transformation function
           \param error function processing if a not interesting element of the optimistic positive border is "almost interesting"
    	*/
        template< class Predicate, class Error, class f> 
    	int processOptGenSub(  Predicate * pred, PTree< SetsType, Measure > * complSet, PTree< SetsType, Measure >  * opt, PTree< SetsType, Measure > * tr, PTree< SetsType, Measure > * freqTr,
    							PTree< SetsType, Measure > * subSet,  int size, f & wordToSet , Error * error  );
    	
        template< class Predicate, class Error, class f>
    	bool processOptGenSub( Predicate  * pred,  Node< SetsType, Measure > * currNode, vectUI *itemset, PTree< SetsType, Measure > * opt,
    		PTree< SetsType, Measure > * complSet, PTree< SetsType, Measure > * tr,  PTree< SetsType, Measure > * freqTr ,  PTree< SetsType, Measure > * subSet, int size, f & wordToSet , Error * error );
    
    	//! Method that move a minimal transversal which have "generated" an interesting set from the set of minimal transversals to another set.
        /**
            \param itemset set wich have generated an interesting element
            \param tr set of the minimal transversals (those which have generated not interesting elements)
            \param freqTr set of the minmal tranversals which have generated an interesting elements
        */	
    	void moveTransFreq( vectUI *itemset, PTree< SetsType, Measure > * tr,  PTree< SetsType, Measure > * freqTr ) ;
 
    	//! Prune the not interesting elements generated by the levelwise exploration.
    	/**
    	    \param pred the predicate
    	    \param tr set of the candidates generated by the levelwise exploration
    	    \param level current level of the levelwise exploration
    	    \param lastBdNElem elements (iterators on) of the negative border found in the current iteration
    	    \param wordToset transformation function
	    */
        template< class Predicate, class f >
    	int pruneCandidates( Predicate  * pred, PTree< SetsType, Measure > * tr, int level, deqItCand & lastBdNElem, f & wordToSet);

        template< class Predicate, class f >
    	int pruneCandidates( Predicate * pred, int level, Node< SetsType, Measure > * currNode, int depth, vectUI *itemset, deqItCand & lastBdNElem, f & wordToSet );
        
    	//! Method generating the candidates of the levelwise exploration.
    	/**
    	 	Actually, this method delete all the subsets generated when processing the optimistic positive border
            which are included into one element of Bd+.
            
            \param subSet  subsets of size k+1 of the not interesting elements find in the optimistic positive border.
    	    \param inbdp the positive border (all the elements of Bd+ found until the current iteration) 
        */
    	void genCand( PTree< SetsType, Measure > * subSet, PTree< SetsType, Measure > * inbdp ) ;
        	
    	bool genCand( Node< SetsType, Measure > * subSet, Node< SetsType, Measure > * bdp, PTree< SetsType, Measure > * tr, int & pos ) ;
    	    	
        //! Method used to do the optimist approach based on "almost interesting" elements
        /**   
    	    From a set of sets in parameter, it founds all the maximals interesting sets 
    	    of the sub lattice until a given level. the exploration of the search space id done level by level.
    	    
    	    \param pred the predicate
    	    \param optIt set containing the almost interesting elements (strored by size)
    	    \param lvl the "smallest" level to explore
    	    \param wordToset transformation function
        */
        template< class Predicate, class f >
    	void optApproach( Predicate  * pred,  set< PTree< SetsType, Measure > > * optIt,  int lvl, f & wordToSet ) ;
    
    	void optApproach( Node< SetsType, Measure > * currNode,  vectUI * itemset, PTree< SetsType, Measure > * nextLev) ;
    
        template< class Predicate, class f >
    	void optApproach( Predicate  * pred, PTree< SetsType, Measure > * opt,  int lvl, f & wordToSet  ) ;
    
    	void initOptApproach( Node< SetsType, Measure > * currNode,  vectUI * itemset, set< PTree< SetsType, Measure > > * optIt) ;
        
        //! "Prune" the interesting elements when exploring almost interesting elements.
        /**
            The interesting elements are pruned for further exploration but stored in the positive border.
            
            \param pred the predicate
            \param tr the set of candidates generated by the exploration
            \param wordToset transformation function
    	*/
        template< class Predicate, class f >
    	int pruneCandidatesOpt( Predicate  * pred, PTree< SetsType, Measure > * tr, f & wordToSet  );
    
    	template< class Predicate, class f >
    	int pruneCandidatesOpt( Predicate  * pred, Node< SetsType, Measure > * currNode, vectUI *itemset, f & wordToSet  );
    	
        
        	
             
    protected:

        typedef typename set< Cand_DataStruct >::iterator setPTreeIt;     
        //typedef typename Cand_DataStruct::iterator ItCand;     
        
        //! Algorithm apriori used for the bottom up initialization
        Apriori< SetsType, Measure,  Cand_DataStruct > apriori;    
        
        //! Negative border
        Cand_DataStruct * bdN ;    
        
        //! Positive border
        Cand_DataStruct * bdP ;   
/*                   	
    	//! Elements of the positive border found by apriori
    	Cand_DataStruct * bdPapriori ;
    	
    	//! Elements of the negative border found by apriori
    	Cand_DataStruct * bdNapriori ;
*/    	
        //! list of the elements of size 1
    //    deque<SetsType> listElements;    	
        
    public:
        
        //! Boolean to print to screen some informations during the execution (default false)
        bool verbose;         	    	           
	

    public:
    
    	//! Constructor.  
    	Abs(){ bdN = new Cand_DataStruct; bdP = new Cand_DataStruct; verbose=false;  } //bdPapriori = new Cand_DataStruct ;	bdNapriori = new Cand_DataStruct}
    
    	//! Destructor.
    	~Abs(){ verbose= false; if( bdP ) delete bdP; if( bdN ) delete bdN; } //if( bdPapriori ) delete bdPapriori; if( bdNapriori ) delete bdNapriori; }
	
        //! Functor operator that executes the algorithm.
        /**
           \param init functor initializing the interesting items wrt predicate
           \param pred the predicate 
           \param stopDualization functor determining when the apiori execution must stop 
           \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
           \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
           \param wordToset transformation function
        */
        template< class InitFunctor, class Predicate, class StopIteration, class OutputBdP, class OutputBdN, class f >   
        void operator() ( InitFunctor & init, Predicate  & pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN, f & wordToSet  ) 
        {
            executeAlgorithm( init, &pred, stopDualization, outBdP, outBdN, (NoError * ) 0, wordToSet ) ;             
        }    
        
        //! Functor operator that executes the algorithm.
        /**
           \param init functor initializing the interesting items wrt predicate.
           \param pred the predicate 
           \param stopDualization functor determining when the apiori execution must stop 
           \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
           \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
        */
        template< class InitFunctor, class Predicate, class StopIteration, class OutputBdP, class OutputBdN  >   
        void operator() ( InitFunctor & init, Predicate  & pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN ) 
        {
            Id identity;
            executeAlgorithm( init, &pred, stopDualization, outBdP, outBdN, (NoError * ) 0, identity ) ;             
        }  
      

      
};


// ----------------------------------------------------------------------------------------------

//! Execute the algorithm.
/**
   \param init functor initializing the interesting items wrt predicate.
   \param pred the predicate 
   \param stopDualization functor determining when the apiori execution must stop 
   \param bdP output the positive border in the given objet (must have a push_back( container, measure ) method).
   \param bdN output the negative border in the given objet (must have a push_back( container, measure ) method).           
   \param error function processing if a not interesting element of the optimistic positive border is "almost interesting"
*/
template< class SetsType, class Measure ,  class Cand_DataStruct  > template< class InitFunctor, class Predicate, class StopIteration, class OutputBdP, class OutputBdN, class Error, class f  >   
void Abs< SetsType, Measure,  Cand_DataStruct >::executeAlgorithm( InitFunctor & init, Predicate  * pred, StopIteration * stopDualization ,  OutputBdP * outBdP, OutputBdN * outBdN, Error * error,  f & wordToSet )
{    
   
    deqItCand lastBdNElem ; // store the last elements of Bd- find for a given iteration of the algorithm. 
 
    deque< vectUI > deqVec ;          
         
    PTree< SetsType, Measure > * complSet = 0 ;

	PTree< SetsType, Measure > * opt=  new PTree< SetsType, Measure >( 0 );	// store all the itemsets that are closed to the positive border

	PTree< SetsType, Measure > * transv = 0 ;

	PTree< SetsType, Measure > * freqTr = 0 ;
	
	PTree< SetsType, Measure > * trie = 0 ;	

	int nbFreq ; // number of itemset of the optimist border
				 // which are interesting or their error is <= eps

	int totGen ; // total number of itemset in the optimist border
	
	time_t start, end ;

    time( &start );
    
    apriori.verbose = verbose;   
    
    Id identity;   
    
    // execute Apriori
    apriori.executeAlgo( init, pred, none, bdP, bdN, wordToSet , stopDualization, identity, &listInterestItems );                         

   	int dualizelevel = stopDualization->level ; // get the last level explored by Apriori
       
    int nbfitem =  listInterestItems.size() ; // get the number if interesting items
  
    time( &end )  	;
		
	if(verbose)
	{
		cout <<"Apriori : [" << difftime( end, start)<< "s]" << endl  ;
		cout<<"Bd+k-1 size : "<<bdP->size()<<endl;
		cout<<"Bd- size : "<< bdN->size()<<endl;
	}

	if( dualizelevel != 0  ) // If Apriori has been stopped, begin dualizations
	{
      
		// Calculation of the transversals mins
		// from the part of the negative border found using Apriori

		freqTr = new PTree< SetsType, Measure >( 0 ) ;

		time( &start );

        // Minimal transversal computation to construct the optimistic positive border
		transv = bdN->trMinOpt( 2 , nbfitem - dualizelevel ) ; // only sets of size > 2 are considered for the transversal minimal computation
		                                                       // since not interesting items (ie those of size 1) are not considered during the exploration		                                                       
        
        time( &end) ;
           

		while( transv->size()                               // itemsets are generated in the optimistic positive border -> process these itemsets
		       || dualizelevel == stopDualization->level    // no itemsets are generated in the optimistic positive border  ->  test the longest itemset
               || !bdN->size()                              // no itemsets were found in Bd- ->  test the longest itemset
               )  
		{
		     
			if(verbose)
			{
                cout<<endl;
				cout<<"Nb attributes : "<<nbfitem<<" Actual level (k) : "<<dualizelevel<<endl;				
				cout <<"Tr Min : "<< transv->size() <<"   [" << difftime( end, start)<< "s]" << endl  ;
			}

			// Generation of the complementaries, ie construct the optimistic positive border

			time( &start );

			if( transv->size() != 0 )
				complSet = transv->complem( listInterestItems ) ;
			else // no itemsets are generated in the optimistic positive border  ->  test the longest itemset
			{
                complSet =  new PTree< SetsType, Measure >( ) ;
			    complSet->push_back( listInterestItems );
             }

			time( &end );
			if(verbose)
				cout <<"Complement [" << difftime( end, start)<< "s]" << endl  ;
            
            time( &start );
            //Pre processing phase of the predicate
		    pred->preProcessing( *complSet, wordToSet ) ;      
            
            time( &end );
			if(verbose)
				cout <<"Pre processing [" << difftime( end, start)<< "s]" << endl  ;
                  

			// Process of the optimist positive border 
			// ie all the interesting elements update Bd+, 
			// not interesting elements with an error < "eps" are stored in opt,
            // and the others are used to generate the sub sets of bad itemsets (in trie).
			// It also updates the set of transversal that generates max interesting itemsets (ie freqTr).

			totGen = complSet->size() ;

			time( &start );
			trie = new PTree< SetsType, Measure >( ) ;

			nbFreq = processOptGenSub( pred, complSet, opt, transv, freqTr, trie, dualizelevel+1, wordToSet, error  ) ;

            time( &end ) ;
            
            time( &start );
            
            //Post processing phase of the predicate
		    pred->postProcessing( *complSet, wordToSet ) ; 
            
            time( &end );
			if(verbose)
				cout <<"Post processing [" << difftime( end, start)<< "s]" << endl  ;
                  	
			complSet->deleteChildren() ;
			delete complSet ;
			complSet = 0;

			if(verbose)
			{
				cout <<"Bd+opt processing and subsets generation[" << difftime( end, start)<< "s]" << endl  ;
				cout <<" : "<< nbFreq <<" sets are closed or in Bd+ on "<< totGen <<" sets generated"<<endl ;
				cout<<"Bd+ size : "<<bdP->size()<<endl;
				cout<<"Opt size : "<<opt->size()<<endl;
				cout <<"Subsets generated : "<<trie->size()<<endl;
				
			}					          
	
			// we execute the pessimist approach
			// for the sub sets of size k+1 of comlSet
			// for nbLev level 

			if(  trie &&  trie->getHead() && trie->size() )
			{	

				time( &start );
			
				// we prune the itemsets included into an itemset of Bd+

                genCand( trie, bdP) ;
                genCand( trie , opt );

                time( &end );
                if(verbose)
                	cout<<"Generated wrt bd+ and opt: "<<trie->size()<<" [" << difftime( end, start)<< "s]" << endl  ;
                 
                time( &start ); 
                //candidates pre processing
                pred->preProcessing( *trie, wordToSet ) ;                	
                
                time( &end );
    			if(verbose)
    				cout <<"Pre processing [" << difftime( end, start)<< "s]" << endl  ;
                      
				// processing of the candidates
                // interesting elements update the pessimist positive border				
                // not interesting elements are insert in bd- 
                                
				time( &start );
							
				nbFreq = pruneCandidates( pred, trie, dualizelevel+1, lastBdNElem, wordToSet );
				
				time( &end ) ;
				
				time( &start );
				//Post processing phase of the predicate
                pred->postProcessing( *trie, wordToSet ) ; 

                time( &end );
    			if(verbose)
    				cout <<"Post processing [" << difftime( end, start)<< "s]" << endl  ;
                                        
				trie->deleteChildren() ;
				delete trie ;
				trie = 0 ;

				dualizelevel++;

				if(verbose)
                { 
					cout <<"Prune Candidates "<< nbFreq << " [" << difftime( end, start)<< "s]" << endl  ;
					cout <<" bd- k : "<<bdN->size() << endl;
				}
                            				               
			}
			else
			    break;

                     			
			if( transv->getHead() || lastBdNElem.size() ) // dualizelevel-1 == stopDualization->level )   // if we can generate sets that are not
									  // supersets of ones that generated interesting elments
			{   
              
			    time( &start );
				// dualization from the itemset of the negative border found at this iteration

   			    transv->trMinIopt( lastBdNElem , nbfitem - dualizelevel +1 , freqTr ) ; 

				time( &end) ;
				
			}
			
			lastBdNElem.clear();
		}

		freqTr->deleteChildren() ;
		delete freqTr ;
		freqTr = 0;
		transv->deleteChildren() ;
		delete transv;
		transv = 0;

		if(  verbose )
		{
			cout<<"Opt size : "<< opt->size()<<endl;
		}

		// we execute the optimist approach
		// ie we found all the maximals that specialize an itemset of opt

		time( &start );

		optApproach( pred, opt, stopDualization->level, wordToSet ) ;

		time( &end) ;
		if( verbose)
			cout<<"Optimist Approach [" << difftime( end, start)<< "s]" << endl  ;
		

		if( verbose )
			cout<<"Bd+ size : "<<  bdP->size()<<endl; //  + bdPapriori->size()<<endl;
 

	}
		
  	delete opt;
   	opt = 0;

	if( outBdP ) // output the positive border if necessary
	{
            for( typename Cand_DataStruct::iterator it = bdP->begin() ; it != bdP->end() ; ++it )
                outBdP->push_back( *  wordToSet.inverse(it), it.measure() ) ;                            
    }

	
	if( outBdN ) // output the negative border if necessary
	{
            for( typename Cand_DataStruct::iterator it = bdN->begin() ; it != bdN->end() ; ++it )          
                outBdN->push_back( *  wordToSet.inverse(it), it.measure() ) ;                            
    }

		
}

// ----------------------------------------------------------------------------------------------

//! Process the optimist positive border
/**
   All the interesting sets update Bd+ and the subests of the not interesting sets are generated.
   If the "almost interesting" elements are studied, this method save these elements in a particular data structure.
   To optimize the minimal transversal computation, minimal transversals which have generated an interesting element are stored separately.
   
   \param pred the predicate 
   \param complSet set of complements of the minimal transversals in the current iteration
   \param opt set used to store the almost interesting elments (not necessarly used)
   \param tr set of the minimal transversals  find until the current iteration
   \param freqtr set of the minimal transversals  find until the current iteration and which have generated an interesting element
   \param subSet strore the subsets of size k (current level of the levelwise approach) of the not interesting elements fond in the optimistic positive border
   \param size current level of the levelwise approach
   \param wordToset transformation function
   \param error function processing if a not interesting element of the optimistic positive border is "almost interesting"
*/
template< class SetsType, class Measure, class Cand_DataStruct > template<class Predicate, class Error, class f>
int Abs< SetsType, Measure, Cand_DataStruct >::processOptGenSub(  Predicate  * pred, PTree< SetsType, Measure > * complSet, PTree< SetsType, Measure >  * opt, PTree< SetsType, Measure > * tr, PTree< SetsType, Measure > * freqTr,
							 PTree< SetsType, Measure > * subSet, int size, f & wordToSet , Error * error  )
{

	int nbFreq;	// number of elements deleted 
				// ie number of interesting elements or near the positive border

	vectUI * tmp ;
  
	tmp = new vectUI ;
    
    if( ! this->bdP )
        bdP = new PTree< SetsType, Measure >();

	nbFreq = bdP->size() + opt->size() ;



	if( complSet->getHead() )
        processOptGenSub( pred, complSet->getHead(),tmp, opt, complSet, tr, freqTr, subSet, size, wordToSet , error  );
	
	nbFreq = bdP->size() + opt->size()  - nbFreq ;

	delete tmp;

	return nbFreq;

}

template< class SetsType, class Measure,  class Cand_DataStruct> template<class Predicate, class Error, class f >	
bool Abs< SetsType, Measure, Cand_DataStruct >::processOptGenSub(  Predicate * pred, Node< SetsType, Measure > * currNode, vectUI *itemset, PTree< SetsType, Measure > * opt,
							   PTree< SetsType, Measure > * complSet, PTree< SetsType, Measure > * tr ,PTree< SetsType, Measure > * freqTr, PTree< SetsType, Measure > * subSet, int size, f & wordToSet , Error * error  )
{
 	
	if( currNode == 0) return 0;

	bool del = true;

	
	for( int i = 0 ; i < currNode->getCnts()->size() ; i++ )
	{
	    if( currNode->existVal( i ) )
        { 
            
            itemset->push_back( currNode->getOffset() + i );
     
     		if( ! currNode->getChilds() || i >= currNode->getChilds()->size()
                             || ! (* currNode->getChilds() )[ i ] )
            {   

                ItCand it( currNode, i+currNode->getOffset(), &complSet->recode,  itemset->size() );      

    			if( !(*pred)( wordToSet.inverse(it), it.measure() ) ) 
    			{                    
    				if( error && (*error)( wordToSet.inverse(it) ) ) // we are closed to the positive border
    				{	
            		
    					// the itemsets for the optimist approach
    					// are inserted stored in opt
    					opt->insert( itemset ) ; 
    					
    					// we move the transversal assoicated with this itemset
    					// to the subset freqTr
    					// it avoid us to create subset of this itemset
    
    					moveTransFreq( itemset, tr, freqTr )  ;
    
    				
    				}
    				else // not interesting elements (and not almost intersting) are used to generate subsets
    					 // for the candidate generation

    					subSet->genSubsets( itemset, size );  

    			}
    			else
    			{                    
    				// Update of the positive border 
    				
    				bdP->insert( itemset, currNode->getMeasure( i ) ) ;
  
    				// we move the transversal assoicated with this itemset
    				// to the subset freqTr
    				// it avoid us to create subset of this itemset
    
    				moveTransFreq( itemset, tr, freqTr )  ;  

    			}
    			
    			// we virtually delete the item
  				// an explicit deletion is not made to avoid
                // unnecessary reallocation
                // since the complSet trie is deleted after 
                // the execution of the method
                          
                currNode->setCnt( i, Measure() ) ;                    		
                        
       			complSet->Decrement() ;	// we decrement the number of itemset in complSet 

   						
    		}
    		else
    		{
    			del = processOptGenSub( pred, (* currNode->getChilds() )[ i ], itemset, opt, complSet, tr, freqTr,
    									subSet, size, wordToSet , error  );
    			if(	( del == true ) &&     
    				( ! currNode->getChilds() || i >= currNode->getChilds()->size()
                             || ! (* currNode->getChilds() )[ i ] ) )    			          	    
       			// if all the children have beene deleted we delete the father node
    			{
    			    // we virtually delete the item
      				// an explicit deletion is not made to avoid
                    // unnecessary reallocation
                    // since the complSet trie is deleted after 
                    // the execution of the method

                    currNode->setCnt( i, Measure() ) ; 			

    			}    		
    		}
    	
    		itemset->pop_back() ;
    	}

   	}    	

	return del;
}

// ----------------------------------------------------------------------------------------------

//! Method that move a minimal transversal which have "generated" an interesting set from the set of minimal transversals to another set.
/**
    \param itemset set wich have generated an interesting element
    \param tr set of the minimal transversals (those which have generated not interesting elements)
    \param freqTr set of the minmal tranversals which have generated an interesting elements
*/	
template< class SetsType, class Measure,  class Cand_DataStruct >
void Abs< SetsType, Measure,  Cand_DataStruct >::moveTransFreq( vectUI *itemset, PTree< SetsType, Measure > * tr,  PTree< SetsType, Measure > * freqTr ) 
{
	vectUI * comp = 0;
		
	// we search the transversal associated with this itemset


	comp = tr->getHead()->complem( itemset, tr->recode( listInterestItems) );

    if( comp && comp->size() )
    {
        // we insert it in the set of transversal that 
        // enable to obtain an interesting element

        freqTr->insert( comp ) ;


    	// we delete this transvresal from the original set
    
    	tr->deleteIt( comp ) ;
     }

	delete comp;

}
	
// ----------------------------------------------------------------------------------------------

//! Prune the not interesting elements generated by the levelwise exploration.
/**
    \param pred the predicate
    \param tr set of the candidates generated by the levelwise exploration
    \param level current level of the levelwise exploration
    \param lastBdNElem elements (iterators on) of the negative border found in the current iteration
    \param wordToset transformation function
*/
template< class SetsType, class Measure, class Cand_DataStruct > template< class Predicate, class f>
int Abs< SetsType, Measure, Cand_DataStruct >::pruneCandidates(Predicate * pred, PTree< SetsType, Measure > * tr, int level, deqItCand & lastBdNElem , f & wordToSet)
{
  int pruned;

  vectUI * tmp = new vectUI( level ) ;
 
  if( !tr->getHead() ) return  0 ;
 
  pruned = pruneCandidates(pred, level,tr->getHead(),1,tmp,lastBdNElem , wordToSet) ;
  
  if( ! tr->getHead()->getCnts() )
  {
      tr->getHead()->deleteChildren() ;
      delete tr->getHead();
      tr->setHead( 0 ) ;
  }    

  tr->setSize( pruned ) ;

  delete  tmp;

  return pruned;
}

template< class SetsType, class Measure,  class Cand_DataStruct > template< class Predicate, class f>
int Abs< SetsType, Measure, Cand_DataStruct >::pruneCandidates( Predicate  * pred, int level,  Node< SetsType, Measure > * currNode, int depth, vectUI *itemset, deqItCand & lastBdNElem , f & wordToSet )
{
  if(currNode == 0) return 0;
  int left = 0;

  for( int i = 0 ; currNode->getCnts()&& i < currNode->getCnts()->size() ; i++ )
  {
     if( currNode->existVal( i ) )
     { 
      
       (*itemset)[depth-1]=  i + currNode->getOffset() ;

        if(depth == level)
        {          
              ItCand it( currNode, (*itemset)[depth-1] , &bdP->recode , level ) ;  

              if( ! (*pred)(  wordToSet.inverse(it), it.measure() ) ) 
              {                 
        		// Update of the negative border 
        
            	Node<SetsType, Measure> * last = bdN->insert( itemset, currNode->getMeasure(i)  ) ;

                ItCand itlast(last , (*itemset)[depth-1], 0, level  ) ;
                lastBdNElem.push_back( itlast );

                if( mem > 0 )
                 currNode->setCnt( i, 0 ) ;
                else
                 i = currNode->deleteItVect( i )  ;  

              }
              else
        		left++;
        }
        else
        {
             int now = pruneCandidates(pred, level, (* currNode->getChilds() )[ i ], depth+1, itemset,lastBdNElem,  wordToSet);
      
              if(now)
                left += now;
              else
              {        
                if( mem > 0 )
                 currNode->setCnt( i, 0 );
                else
                 i = currNode->deleteItVect( i )  ;                   
              }   
        }
     }   

  }
  return left;
}

// ----------------------------------------------------------------------------------------------    

//! Method generating the candidates of the levelwise exploration.
/**
 	Actually, this method delete all the subsets generated when processing the optimistic positive border
    which are included into one element of Bd+.
    
    \param subSet  subsets of size k+1 of the not interesting elements find in the optimistic positive border.
    \param inbdp the positive border (all the elements of Bd+ found until the current iteration) 
*/
template< class SetsType, class Measure,  class Cand_DataStruct >
void Abs< SetsType, Measure,Cand_DataStruct >::genCand( PTree< SetsType, Measure > * subSet, PTree< SetsType, Measure > * inbdp )
{
	bool del = false ;

	setPTreeIt tmpIt ;

	if( subSet != 0 && inbdp != 0 && subSet->getHead() )
	{
		for( int i = 0; i < subSet->getHead()->getCnts()->size() ; i++ )
		{
	
			if( subSet->getHead()->existVal( i ) )
			{
    	        del=genCand( subSet->getHead() , inbdp->getHead(), subSet, i ) ; 

    			if( del && ! subSet->getHead()->getCnts() )
    			{
                           delete subSet->getHead() ;
                           
                           subSet->setHead( 0 ) ;
                           break;
                }    
    		}
  		}
  		  		
	}
}

template< class SetsType, class Measure,  class Cand_DataStruct >
bool Abs< SetsType, Measure, Cand_DataStruct >::genCand( Node< SetsType, Measure > * subSet, Node< SetsType, Measure > * bdp, PTree< SetsType, Measure > * tr, int & pos )
{
    
   	bool del = false ;

   	int index ;
   	
	if( subSet == 0 || bdp == 0 ) return del;


    index = pos + subSet->getOffset() - bdp->getOffset() ;


    if( index >= 0  ) // is index < 0 the current item in subset can't be in bdp
	{

	    if( index < bdp->getCnts()->size() &&  bdp->existVal( index ) ) // the item is in bdp
	    {
        
        	if( ! subSet->getChilds() || pos >= subSet->getChilds()->size() || ! (* subSet->getChilds() )[ pos ] ) 
    		{

    			del = true ;
                if( mem > 0 )
                 subSet->setCnt( pos, 0 ) ;
                else
                                 pos = subSet->deleteItVect( pos )  ;  
    			
    			tr->Decrement() ;
    
       		}
    		else // we don't have reached an itemset of subset
          	{    

          	   if(  bdp->getChilds() && index < bdp->getChilds()->size() 
                                && (* bdp->getChilds() )[ index ] )
               {

                    Node< SetsType, Measure > * child = (* subSet->getChilds() )[ pos ] ;  

                	for( int i = 0 ; i < child->getCnts()->size() ; i++ )
                	{       

                	    if( child->existVal( i )  )
                	    {                                         	        
                	       del = genCand(  child, ( * bdp->getChilds() )[ index ], tr, i );
                     	                            	  
                	    }  

                         if( del && ! child->getCnts() )
                	    {
                            if( mem > 0 )
                             subSet->setCnt( pos, 0 ) ;
                            else
                             pos = subSet->deleteItVect( pos )  ;  
                               
                            return del;
                        }    
                	}                                                                                   
           	    }          	    
          	} 
        }
        

        if(  subSet->getCnts() )
        {
            if( bdp->getChilds() )
            {
                for( int j = 0 ;  j < index &&  j < bdp->getChilds()->size() && !( del && 
                    	( ! subSet->getCnts() || pos >= subSet->getCnts()->size()
                         || ! subSet->existVal( pos )  ) ) 			          	    
                 ; j++ )
                 {

                     if(  (* bdp->getChilds() )[ j ] )        
                          del = genCand(  subSet, (* bdp->getChilds() )[ j ], tr, pos  );

                 }    
            }           
        }    
    }          			      		    
			
    return del;
}

// ----------------------------------------------------------------------------------------------       
        	    
//! Method used to do the optimist approach based on "almost interesting" elements
/**   
    From a set of sets in parameter, it founds all the maximals interesting sets 
    of the sub lattice until a given level. the exploration of the search space id done level by level.
    
    \param pred the predicate
    \param optIt set containing the almost interesting elements (strored by size)
    \param lvl the "smallest" level to explore
    \param wordToset transformation function
*/
template< class SetsType, class Measure,  class Cand_DataStruct > template< class Predicate, class f>
void Abs< SetsType, Measure, Cand_DataStruct >::optApproach( Predicate  * pred,  set< PTree< SetsType, Measure > > * optIt, int lvl, f & wordToSet  ) 
{
	vectUI * itemset ;

	setPTreeIt runner ;

	setPTreeIt runner2 ;

	setPTreeIt tmp ;
	
	PTree< SetsType, Measure > tmpSet(0) ;

	int nbfreq ;
	
	int level ;

	if( ! optIt->empty() )
	{
		itemset = new vectUI() ;

		// we search the actual the level
		// ie we find the itemsets with the max size

		runner = optIt->end() ;

		runner --;

		runner2 = runner  ;
		
		while( ( ! optIt->empty() ) && runner->getHead() && ( runner->length() > lvl )    )
		{	

			level = runner->length() ;

			if(verbose)
				cout <<"Optimist approach : level "<<level-1<<endl;

			// if the sub set of the current itemsets
			// don't have the same level than the next PTree< SetsType, Measure >
			// we create a new sub set in this PTree< SetsType, Measure >
			// according to the level
			
			if( ( runner2 != optIt->begin() ) && ( ( --runner2 )->length() == level - 1 ) )
			{
		
				tmpSet.setHead( runner2->getHead() ) ;	
				tmpSet.setHeight( runner2->length() );
				tmpSet.setSize( runner2->size() );
		    }				
			else
			{
				tmpSet.setHead( new Node< SetsType, Measure >() ) ;

				tmpSet.setHeight( level - 1 ) ;

				optIt->insert( tmpSet ).first ;
			}

			clock_t start = clock() ;
  
			// generation of the sub set of the itemsets of the current itemsets in optIt

			optApproach( runner->getHead(), itemset, &tmpSet ) ;

			if(verbose)
				cout <<"\t subests generation : "<<tmpSet.size() <<"\t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
			
			start = clock();
			
			// we prune the itemsets included into an itemset of Bd+
		
			genCand( &tmpSet, bdP) ;
				
			if(verbose)
				cout<<"\t Generated wrt bd+ an bd+opt : "<<tmpSet.size()<<" [" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			// support update
			pred->preProcessing( tmpSet, wordToSet ) ;


			if(verbose)
				cout <<"\t Supports update \t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;

			// we delete the old not interesting element

			PTree< SetsType, Measure > * tmpT = const_cast< PTree< SetsType, Measure > * >( &(*runner ) );
			tmpT->deleteChildren() ;
			tmp = runner-- ;
			optIt->erase( tmp ) ;

			start = clock();

			// prune the interesting elements
			// and update of the positive and negative border 

			nbfreq = pruneCandidatesOpt( pred,  &tmpSet,  wordToSet ) ;

			// To keep in runner the actual number of itemets
			tmpT = const_cast< PTree< SetsType, Measure > * >( &(*runner ) );
			tmpT->setSize( tmpSet.size() );
			tmpT->setHeight( tmpSet.length() );
			
			tmpT->setHead( tmpSet.getHead() ) ;
			
			if(verbose)
			{
				cout <<"\t pruning interesting elements and update Bd+opt \t[" << (clock()-start)/double(CLOCKS_PER_SEC) << "s]" << endl;
				cout <<"\t number of interesting elements : "<<nbfreq <<endl;
				cout <<"\t Bd+ :"<<this->bdP->size()<<endl; ;
			}				
		
			runner2 =  runner ;

		}

		delete itemset ;
	}
}

template< class SetsType, class Measure,  class Cand_DataStruct >
void Abs< SetsType, Measure,  Cand_DataStruct >::optApproach( Node< SetsType, Measure > * currNode,  vectUI * itemset, PTree< SetsType, Measure > * nextLev) 
{

	if( currNode )
	{
    	for( int i = 0 ; i < currNode->getCnts()->size() ; i++ )
    	{
    	    if( currNode->existVal( i ) )
            { 
                // we save the curent item of the itemset being processed
        
     			itemset->push_back( i + currNode->getOffset() ) ;			
        			
                
          		if( currNode->getChilds() && i < currNode->getChilds()->size() 
                        && (* currNode->getChilds() )[ i ] )
                {	    			        			
        			// we continue searching with the next items
        
        			optApproach( (* currNode->getChilds() )[ i ], itemset, nextLev ) ;
        
        			// deletion of the itemset
        
        			// we virtually delete the item
      				// an explicit deletion is not made to avoid
                    // unnecessary reallocation
                    // since the trie is deleted after 
                    // the execution of the method
                          
                    currNode->setCnt( i, 0 ) ; 
        					
        
         		}   
                else	// we have check all the items of the current itemset strored in vect			
                		// we generate the immediate subsets of the itemset
                		
                		nextLev->genSubsets( itemset, itemset->size() - 1 ) ;
                		
       			itemset->pop_back() ;
   
      		}
        }  		
    	
	}
	

}

template< class SetsType, class Measure,  class Cand_DataStruct > template< class Predicate, class f>
void Abs< SetsType, Measure,  Cand_DataStruct >::optApproach( Predicate  * pred, PTree< SetsType, Measure > * opt,  int lvl, f & wordToSet   ) 
{
	vectUI * vect ;

	set < PTree< SetsType, Measure > > * 	optIt= new set< PTree< SetsType, Measure > >() ;
	
	if( opt->getHead() != 0 )
	{
		vect = new vectUI() ;

		initOptApproach( opt->getHead() , vect, optIt ) ;

		delete vect ;

		optApproach( pred, optIt,  lvl, wordToSet ) ;

	}

	for( setPTreeIt runner = optIt->begin(); runner != optIt->end(); runner++ )
	{
	      PTree< SetsType, Measure > * tmpT = const_cast< PTree< SetsType, Measure > * >( &(*runner ) );
	    
          tmpT->deleteChildren();
	}	
	delete optIt ;
	

		
}

template< class SetsType, class Measure,  class Cand_DataStruct >
void Abs< SetsType, Measure, Cand_DataStruct >::initOptApproach( Node< SetsType, Measure > * currNode,  vectUI * itemset, set< PTree< SetsType, Measure > > * optIt   ) 
{
   	setPTreeIt tmp ;

	if( currNode )
	{
    	for( int i = 0 ; i < currNode->getCnts()->size() ; i++ )
    	{
    	    if( currNode->existVal( i ) )
            { 
                // we save the curent item of the itemset being processed
        
     			itemset->push_back( i + currNode->getOffset() ) ;			
        			
                
          		if( currNode->getChilds() && i < currNode->getChilds()->size() 
                        && (* currNode->getChilds() )[ i ] )
                {	    			        			
        
            		// we continue searching with the next items
        
        			initOptApproach( (* currNode->getChilds() )[ i ], itemset, optIt ) ;

       
        			// deletion of the itemset
        
        			// we virtually delete the item
      				// an explicit deletion is not made to avoid
                    // unnecessary reallocation
                    // since the trie is deleted after 
                    // the execution of the method
                          
                    currNode->setCnt( i, 0 ) ; 
                    
          		}   
                else	// we have check all the items of the current itemset strored in vect			
       			{       // construction of the trees that regroup itemsets wrt their size
            	
            		tmp = optIt->find( PTree< SetsType, Measure >( 0,0, itemset->size() ) );
            
            		if( tmp == optIt->end() )         		        
            				tmp = optIt->insert( PTree< SetsType, Measure >(0,0, itemset->size() ) ).first ;
           
            		// the itemsets for the optimist approach
            		// are inserted into the good PTree< SetsType, Measure > of optIt
            
            		PTree< SetsType, Measure > * tmpT = const_cast< PTree< SetsType, Measure > * >( &(*tmp ) );
            		tmpT->insert( itemset ) ;
            	}

                		
       			itemset->pop_back() ;
   
      		}
        }  		
    	
	}
}

// ----------------------------------------------------------------------------------------------

//! "Prune" the interesting elements when exploring almost interesting elements.
/**
    The interesting elements are pruned for further exploration but stored in the positive border.
    
    \param pred the predicate
    \param tr the set of candidates generated by the exploration
    \param wordToset transformation function
*/
template< class SetsType, class Measure,  class Cand_DataStruct > template< class Predicate, class f>
int Abs< SetsType, Measure,Cand_DataStruct >::pruneCandidatesOpt( Predicate  * pred, PTree< SetsType, Measure > * tr, f & wordToSet  )
{
	int nbnf =0 ;
	int nbfreq =0 ;

	vectUI * tmp ;

  	if( !tr->getHead()  ) return  0 ;

	tmp = new vectUI ;

	nbfreq = tr->size() ;

	nbnf = pruneCandidatesOpt( pred, tr->getHead() , tmp,  wordToSet );

    if(  ! tr->getHead()->getCnts() )
    {
        tr->getHead()->deleteChildren() ;
        delete tr->getHead();
        tr->setHead( 0 ) ;
    }    
	
	tr->setSize(  nbnf ) ;

	nbfreq = nbfreq -  nbnf ;

	delete tmp;

	return nbfreq;

}
	
template< class SetsType, class Measure, class Cand_DataStruct > template< class Predicate, class f>
int Abs< SetsType, Measure, Cand_DataStruct >::pruneCandidatesOpt( Predicate  * pred, Node< SetsType, Measure > * currNode, vectUI *itemset, f & wordToSet  )
{

	setPTreeIt runDel ;

	if(currNode == 0) return 0;

	int left = 0;

	for( int i = 0 ; currNode->getCnts()&& i < currNode->getCnts()->size() ; i++ )
    {
         if( currNode->existVal( i ) )
         { 
    		itemset->push_back( i + currNode->getOffset() );
    
    		// if we have find an itemset
    
    		if( ! currNode->getChilds() || i >= currNode->getChilds()->size()
                             || ! (* currNode->getChilds() )[ i ] )
            {       
    			// we keep the not interesting element in the set
    			// we update tmp bd+ with the interesting element
    			// and delete them from the set
    			// since we search the maximals interesting elements
    
                ItCand it( currNode, i+currNode->getOffset(), 0, itemset->size() ) ;
    			if( (*pred)(  wordToSet.inverse(it) , it.measure()  ) ) 
    			{
    
    				// we insert the interesting element in  Bd+ of the optimist approach
    				
    				bdP->insert( itemset, currNode->getMeasure( i ) ) ;
    
    				// the itemset is  interesting 
    				// so we delete it
                    if( mem > 0 )
                     currNode->setCnt( i, Measure() ) ;
                    else
                     i = currNode->deleteItVect( i )  ;  
                }
    			else
    				left++;
    			
    	    }
    		else
    		{
    			int now = pruneCandidatesOpt( pred,(* currNode->getChilds() )[ i ] , itemset,  wordToSet );
    
    			if(now)
    				left += now;
    			else 
    			{
                    if( mem > 0 )
                     currNode->setCnt( i, Measure() ) ;
                    else
                     i = currNode->deleteItVect( i )  ;  
    			}    

    		}
    		itemset->pop_back() ;
  		}  		
	}

	return left;
}

// ----------------------------------------------------------------------------------------------  




#endif
