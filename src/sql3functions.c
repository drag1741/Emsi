/*sql3functions.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>

#include "../sqlite3/sqlite3.h"
#include "sql3functions.h"

//commands for creating sqlite3 tables 
char *create_dataline_table="CREATE TABLE dataline( ID INT PRIMARY KEY NOT NULL,"
                                                  "body TEXT, title TEXT, expired DATE,"
                                                  "posted DATE, state TEXT, city TEXT,"
                                                  "onet TEXT, soc5 TEXT, soc2 TEXT);";
char *create_map_onet_soc_table="CREATE TABLE map_onet_soc( onet TEXT, soc5 TEXT);";
char *create_soc_hierarchy_table="CREATE TABLE soc_hierarchy(child TEXT, parent TEXT,"
                                                            "level INT, name TEXT);";
/*open and/or create sqlite3 database dataline.db
  create three tables
  1.dataline-each row is data from a line in data_in_file
  2.map_onet_soc-mapping of onet to soc5
  3.soc_hierarchy-holds child,parent,level and name information for each soc5 code
*/
void open_sqlite3_db(sqlite3 *data_line_db){
    //open sqlite database
    char *err_message = 0;
    int rc = sqlite3_open("dataline.db", &data_line_db);
    if(rc != SQLITE_OK){//sqlite3 db failed to open
        fprintf(stderr,"[error] data_line_db sqlite3 db failed to open");
        sqlite3_close(data_line_db);
        exit(-3);
    }
    sqlite3_exec(data_line_db,create_dataline_table,0,0, &err_message);
    sqlite3_exec(data_line_db,create_map_onet_soc_table,0,0, &err_message);
    sqlite3_exec(data_line_db,create_soc_hierarchy_table,0,0, &err_message);
}
