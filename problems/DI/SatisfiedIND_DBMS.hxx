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
 
#ifndef SATISFIEDIND_DBMS_HXX
#define SATISFIEDIND_DBMS_HXX

#include<set>
#include<utility>

#include "Predicate.hxx"

//! Functor representing the predicate being a satisfied IND wrt to 2 relations in a SGBD. 
/*!
    This functor test if a IND is satisfied for two relation stored in a DBMS.
    Note that each time the functor is used, a query is executed on the database.   
    
    The template parameter DBMS is the class representing the connection to the DBMS.
    This class is used to do operations on the DBMS such as connect, execute a query ...
    
    The attributes in the two relations must have the same data type.
*/
template< class DBMS>
class SatisfiedIND_DBMS: public Predicate 
{              
    protected:
              
        //! Pointer on the dbms
        DBMS * mydbms;

            
    public:  
                    
        //! Constructor
        /*!
            \param inDbms pointer on the DBMS and the database studied
        */
        SatisfiedIND_DBMS( DBMS * inDbms )
        { 
                mydbms = inDbms ;  
                
                if( mydbms->get_relation(1) && mydbms->get_relation(2) )
                {                                     
                    // store a parameretrized query on the two relations to test the dependencies
                    string query = "select count(*) from "+  mydbms->get_relation(1)->name + " where ( var1 ) not in( SELECT distinct var2 FROM "+  mydbms->get_relation(2)->name +" )" ;                
                    mydbms->store_query( (char *)(query.c_str()) );
                }
                
        }
 
        //! Destructor
        ~SatisfiedIND_DBMS(){}            
        
        //! Operator that test if a set of attributes is not redundant
        template< class Iterator, class Measure >
        bool operator() ( Iterator  itCand, Measure & mesCand )
        {       

             //search the attributes in the left part of the IND
             string left= itCand->left[0] ;
             for( int i = 1; i < itCand->left.size(); i++)
                  left+=","+itCand->left[i] ;

                                                
             //search the attributes in the right part of the IND
             string right= itCand->right[0] ;
             for( int i = 1; i < itCand->right.size(); i++)
                  right+=","+itCand->right[i] ;                  

             left= " "+left+" ";
             right=" "+right+" ";
             
             //replace the variable by the attribute of the IND being studied     
             mydbms->replace_in_query(" var1 ", (char *)(left.c_str()) );
             mydbms->replace_in_query(" var2 ", (char *)(right.c_str()) );

             //execute the query
             string nb_notin = mydbms->exec_query();

             //re set the variables for the next predicate test
             mydbms->replace_in_query( (char *)(left.c_str()), " var1 " );
             mydbms->replace_in_query( (char *)(right.c_str()), " var2 " );


             if( nb_notin == "0" ) // if there are values not in the second relation (wrt the attributes studied)
                 return true ;    // the IND is not satisfied
             else
                 return false ;
        }
                 
};   

        
#endif 
