/*sql3functions.h
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#ifndef SQL3FUNCTIONS_H
#define SQL3FUNCTIONS_H

//open and/or create sqlite3 database dataline.db
//create three tables
//1.dataline-each row is data from a line in data_in_file
//2.map_onet_soc-mapping of onet to soc5
//3.soc_hierarchy-holds child,parent,level and name information for each soc5 code
void open_sqlite3_db(sqlite3 *data_line_db);

#endif