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
 
#ifndef MYSQL_DB_HPP 
#define MYSQL_DB_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <windows.h>

#include <mysql/mysql.h>

using namespace std;

// Class that represents a Mysql relation/table
class Mysql_rel
{
 protected:

      //! store the connection with the mysql database    
      MYSQL * mysql;                       
      
 public:      
 
      //! Constructor    
      /*!
            \param inName name of the relation/table
            \param inMysql pointer to the mysql database
      */    
      Mysql_rel( char * inName,  MYSQL * inMysql )
      { 
        name = inName ; 
        mysql = inMysql; 
        
        // Initialize the list of all attributes
        MYSQL_RES *result;
        MYSQL_ROW row;
        
        string query = "describe "+ name +";";
        
        if( mysql_query( mysql,query.c_str() ) )
        {    
             cerr<<"Error when selecting the relation "<<name; 
        }
        else
        {    
            result = mysql_store_result(mysql);
           
            attributes.reserve( mysql_num_rows(result) );
        
            while( row = mysql_fetch_row(result) )
                   attributes.push_back( row[0] );
            
            mysql_free_result( result );        
        }
    
      }
      
      //! Destructor     
      ~Mysql_rel(){}
      
      //! Name of the relation
      string name;
      
      //! List of all the attributes.
      vector<string> attributes;               
      
};      



// Class that represents a Mysql database
class Mysql_db
{   
      typedef map<char *,Mysql_rel>   mapRel;      
      typedef map<char *,Mysql_rel>::iterator  mapRelIt;  
                   
 protected:
      
      //! Store the connection with the mysql database    
      MYSQL mysql;
      
      //! List of the  already selected relations.
      /*
          used to avoid the execution of a query to get the structure of a relation already tested.
      */
      map<char *, Mysql_rel > selRelations;   
      
      //! Vector used to store the ordering of selection of the tables (the first selection of each relation)
      vector< Mysql_rel *> order_selRel ;
      
      //! Query stored
      string query;
      
      
 public:                                     
      
      //! Constructor
      Mysql_db( char * host_name, char * db_name, char * login, char * pwd )
      {   
            mysql_init(&mysql);
           
            if (!mysql_real_connect(&mysql,host_name,login,pwd, db_name, 0, NULL, 0)) 
            {                                                            
                mysql_close(&mysql);                                             
                cerr<<"Connect error";
            }

      }   
      
      //! Destructor
      ~Mysql_db(){ mysql_close(&mysql); }
      
      //! Get the stored query
      string & get_query(){ return query ;}
      
      //! Select a relation/table in the database
      /*!
          \param relation_name name of the relation/table to select.
          \return the selected relation.
      */
      Mysql_rel & select_relation( char * relation_name )
      {
            mapRelIt it ;
            pair<mapRelIt, bool> itinsert;
            
            it = selRelations.find( relation_name ) ;
            
            if( it == selRelations.end() )  // if not already selected store this relation
            {       
                itinsert = selRelations.insert( mapRel::value_type( relation_name, Mysql_rel( relation_name,  &mysql ) ) );                   
                it = itinsert.first;
                
                order_selRel.push_back( &(it->second) ); // store the order of the selected realtion
            }

            return  it->second;
      }
      
      //! Get a pointer on the ith selected relation
      Mysql_rel * get_relation( int i )
      { 
                i = i -1;          
                if( i<0 && i >= order_selRel.size() ) return 0; 
                return order_selRel[i] ; 
      }
      
      //! Store a query
      /*!
          note that it does not store the result of the query
      */     
      void store_query( char * inQuery  ) { query = inQuery; }
      
      //! Replace some part of the query by another character string
      /*!
         \param old_string character string to replace in the stored query
         \param new_string  new character string to insert in the stored query
      
         This methode could be used to paramatrised the query.
         For example, store the query "select var0 from Emp;".
         Then at runtime, replace "var0" by another character string 
         (a variable, from a keyboard input...)   
         Consequently, the query is can be parameterized.    
      */
      void replace_in_query( char * old_string, char * new_string)
      {           
           unsigned int pos = 0; // current position in character string
           
           string oldstr = old_string;
           string newstr = new_string ;
           
           while( (pos = query.find(oldstr, pos+1 ) ) != string::npos)
                  query.replace( pos, oldstr.size(), newstr);
      }
      
      //! Execute the stored query (works only if the query returns an unique value).
      /*!
          \return the result value of the query
          
          Note that this method only works if the query returns an unique value,
          (the query is such as "select count .." or "select min.." ).
          If it is not this type of query, this function returns the value of 
          the first attribute of the first the tuple of the query result.
      */
      string exec_query()
      {
            MYSQL_RES *result;
            MYSQL_ROW row;   
            string res;
            
            mysql_query( &mysql, query.c_str() ); // execute the query

            result = mysql_use_result(&mysql); 
            row = mysql_fetch_row(result); // get the first row of the result
            res = (char *)(row[0]);
                         
            mysql_free_result( result );  
            
            return res  ;
               
      }
      
    
      
      
      
};
#endif
