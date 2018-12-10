/*sql3functions.h
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#ifndef SQL3FUNCTIONS_H
#define SQL3FUNCTIONS_H

extern FILE *err_log;//error loggin filed created in main.c

//open and/or create sqlite3 database dataline.db
//create three tables
//1.dataline-each row is data from a line in data_in_file
//2.map_onet_soc-mapping of onet to soc5
//3.soc_hierarchy-holds child,parent,level and name information for each soc5 code
void open_sqlite3_db(sqlite3 *dataline_db);
//insert rows into map_onet_soc table from data/map_onet_soc.csv
void fill_map_onet_soc(sqlite3 *dataline_db);
/*****insert rows into soc_hierarchy table from data/soc_hierarchy.csv*****/
void fill_soc_hierarchy(sqlite3 *dataline_db);

#endif
