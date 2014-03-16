#ifndef STOPITEDISTRIB_HXX
#define STOPITEDISTRIB_HXX


//! Functor used in parameter of Apriori to stop the execution of the algorithm wrt distribution of elements found.
/**
    If the threshold is less or equal  than 1, stop the Apriori execution when the number of elments in the negative border found in an iteration on the number of candidates generated is less than a threshold given in input.
    If the threshold is greater than 1, stop when the algorithm has processed level = threshold.
*/
class StopIteDistrib
{
    protected:
              
        //! threshold (if > 1 last level studied).
        int threshold;
        
        //! Number of candidates for the current level at th curretn step of the algorithm
        int nbCand;       
                               
    public:
        
        //! Last level studied by the algorithm
        int level;
           
        //! Constructor.   
        StopIteDistrib(  int  inthreshold ) { threshold = inthreshold; nbCand = 0 ; level =0; }
        
        //! Update the informations in the functor wrt the current step of the algorithme.
        /*!
            By updating the informations in the functor we consider the informations discovered at another step of the algorithm.            
            For example, an update is done in Apriori after generating (+ pre processing of the predicate) candidates.
        */
        template< class Cand_DataStruct>
        void update( Cand_DataStruct & candidates )
        {
             nbCand = candidates.size() - nbCand ;             
        }
        
        //! Operator used to determine if the algorithm should stop.
        /**
           Since the test is done after the pruning step of Apriori, we store
           \return true if the ratio on the number of elements of Bd- on the numbre of candidates generated is less thant the thresold.
        */
        template< class Cand_DataStruct>
        bool operator() ( Cand_DataStruct & candidates )
        {    

             level =  candidates.length();
             bool stop = false ;

                    
             if( threshold <= 1 )
             {
                 int nbBdN = nbCand - candidates.size(); // count the number of elements of Bd-
                 
                 if( !nbCand || (nbBdN / nbCand ) < threshold  )
                     stop = true ;
                     
             }
             else if( threshold <= candidates.length() )
                  stop = true ;
                  
             if( ! stop ) 
                 level = 0 ;  // to detect in ABS that Apriori has not been stopped
             return stop ;
        }                  
};

#endif
