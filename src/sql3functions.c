/*sql3functions.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../sqlite3/sqlite3.h"
#include "sql3functions.h"
#include "dataline.h"//needed for FILE *err_log

//commands for creating sqlite3 tables 
const char *create_dataline_table="CREATE TABLE dataline(body TEXT, title TEXT, expired DATE,"
                                                  "posted DATE, state TEXT, city TEXT,"
                                                  "onet TEXT, soc5 TEXT, soc2 TEXT);";
const char *create_map_onet_soc_table="CREATE TABLE map_onet_soc( onet TEXT, soc5 TEXT);";
const char *create_soc_hierarchy_table="CREATE TABLE soc_hierarchy(child TEXT, parent TEXT);";
/*open or create sqlite3 database dataline.db
  create three tables
  1.dataline-each row is data from a line in data_in_file
  2.map_onet_soc-mapping of onet to soc5
  3.soc_hierarchy-holds child,parent,level and name information for each soc5 code
*/
void open_sqlite3_db(sqlite3 *dataline_db){
    //check if tables exist and if not create and fill
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM sqlite_master WHERE name='dataline' AND type='table';";
    sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL);
    if( sqlite3_step(stmt) == SQLITE_ROW ){//dataline table found, drop from database
        fprintf(err_log,"dataline table found...droping table\n"); 
//        sqlite3_exec(dataline_db,"DROP TABLE dataline;",0,0, NULL);
    }
    sqlite3_exec(dataline_db,create_dataline_table,0,0, NULL);//dataline table
    sqlite3_finalize(stmt);

    sql = "SELECT * FROM sqlite_master WHERE name='map_onet_soc' AND type='table';";
    sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL);
    if( sqlite3_step(stmt) != SQLITE_ROW ){//map_onet_soc table
        fprintf(err_log,"mop_onet_soc not found...creating new table\n"); 
        sqlite3_exec(dataline_db,create_map_onet_soc_table,0,0, NULL);
        fill_map_onet_soc(dataline_db);
    }
    sqlite3_finalize(stmt);

    sql = "SELECT * FROM sqlite_master WHERE name='soc_hierarchy' AND type='table';";
    sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL);
    if( sqlite3_step(stmt) != SQLITE_ROW ){//soc_hierarchy table
        fprintf(err_log,"soc_hierarchy not found...creating new table\n"); 
        sqlite3_exec(dataline_db,create_soc_hierarchy_table,0,0, NULL);
        fill_soc_hierarchy(dataline_db);
    }
    sqlite3_finalize(stmt);
}
/*****insert rows into map_onet_soc table from data/map_onet_soc.csv*****/
void fill_map_onet_soc(sqlite3 *dataline_db){
    /****open data/map_onet_soc.csv and check if failed to open, exit if failed****/
    FILE *map_onet_file;
    map_onet_file = fopen("./data/map_onet_soc.csv","r");
    if( map_onet_file == NULL){
        fprintf(err_log, "[error] ./data/map_onet_soc.csv failed to open");
        exit(-1);
    }
    /****create varaibles for reading in onet and soc5 codes and build sqlite3 insert command***/
    char in_onet[13];//holds onet text from file
    in_onet[0] = '"';//needed by sqlite3 command to make string literal
    in_onet[11] = '"';
    in_onet[12] = '\0';//set null terminator
    char in_soc5[10];//holds soc5 text from file
    in_soc5[0] = '"';//needed by sqlite3 command to make string literal
    in_soc5[8] = '"';
    in_soc5[9] = '\0';//set null terminator
    char insert_onet_soc5[100];//sqlite3 insert command
    strcpy(insert_onet_soc5,"INSERT INTO map_onet_soc(onet,soc5) VALUES(");
    char in_char;//char buffer for reading in map_onet_file
    while((in_char = fgetc(map_onet_file)) != '\n');//first line is header of file and not needed
    int i = 1;//index value for in_onet and in_soc5 arrays 
    while((in_char = fgetc(map_onet_file)) != EOF ){ 
        /****read in onet values****/
        if(i < 11){
            in_onet[i] = in_char;
            i++;
        }
        else if( i == 11 ){//when i==11, in_char == ','
            i++;
        }
        /****read in soc5 values****/
        else if( i > 11 && i < 19 ){//when i==19, in_char =='\n'
            in_soc5[i-11] = in_char;
            i++;
        }
        else{
            i = 1;//reset i and for next line read
            //build sqlite3 insert command
            strncat(insert_onet_soc5,in_onet,12);
            strncat(insert_onet_soc5,",",1);
            strncat(insert_onet_soc5,in_soc5,9);
            strncat(insert_onet_soc5,");",2);
            /****insert onet and soc5 into map_onet_soc_table in sqlite3.db****/
            sqlite3_exec(dataline_db,insert_onet_soc5,0,0, NULL);
            insert_onet_soc5[43] = '\0';//reset insert_onet_soc5 for next instruction
        }
    }
    fclose(map_onet_file);
}
/*****insert rows into map_onet_soc table from data/map_onet_soc.csv*****/
void fill_soc_hierarchy(sqlite3 *dataline_db){
    /****open data/map_onet_soc.csv and check if failed to open, exit if failed****/
    FILE *soc_hierarchy_file;
    soc_hierarchy_file= fopen("./data/soc_hierarchy.csv","r");
    if( soc_hierarchy_file== NULL){
        fprintf(err_log, "[error] ./data/soc_hierarchy.csv failed to open");
        exit(-1);
    }
    /***create varaibles for reading in child,parent and build sqlite3 insert command**/
    int child_size = 10;
    char child[child_size];//holds child text from file
    child[0] = '"';//needed by sqlite3 command to make string literal
    child[child_size-2] = '"';
    child[child_size-1] = '\0';//set null terminator
    int parent_size = 10;
    char parent[parent_size];//holds soc5 text from file
    parent[0] = '"';//needed by sqlite3 command to make string literal
    parent[parent_size-2] = '"';
    parent[parent_size-1] = '\0';//set null terminator
    char insert_soc_hierarchy[100];//sqlite3 insert command
    strcpy(insert_soc_hierarchy,"INSERT INTO soc_hierarchy(child,parent) VALUES(");
    char in_char;//char buffer for reading in map_onet_file
    while((in_char = fgetc(soc_hierarchy_file)) != '\n');//first line is header of file and not needed
    int i = 1;//index value for in_onet and in_soc5 arrays 
    while((in_char = fgetc(soc_hierarchy_file)) != EOF ){ 
        /****read in child values****/
        if(i < child_size-2){
            child[i] = in_char;
            i++;
        }
        else if( i == child_size-2 ){//when i==8, in_char == ','
            i++;
        }
        /****read in parent values****/
        else if( i > child_size-2 && i < child_size+parent_size-4 ){//when i==16, in_char =='\n'
            parent[i-(child_size-2)] = in_char;
            i++;
        }
        else{
            i = 1;//reset i and for next line read
            //build sqlite3 insert command
            strncat(insert_soc_hierarchy,child,child_size-1);//exclude null terminator
            strncat(insert_soc_hierarchy,",",1);
            strncat(insert_soc_hierarchy,parent,parent_size-1);//exclude null terminator
            strncat(insert_soc_hierarchy,");",2);
            while((in_char = fgetc(soc_hierarchy_file)) != '\n' );//move to next line
            /****insert onet and soc5 into map_onet_soc_table in sqlite3.db****/
            sqlite3_exec(dataline_db,insert_soc_hierarchy,0,0, NULL);
            insert_soc_hierarchy[47] = '\0';//reset insert_soc_hierarchy for next instruction
        }
    }
    fclose(soc_hierarchy_file);
}
