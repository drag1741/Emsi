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

//commands for creating sqlite3 tables 
char *create_dataline_table="CREATE TABLE dataline( ID INT PRIMARY KEY NOT NULL,"
                                                  "body TEXT, title TEXT, expired DATE,"
                                                  "posted DATE, state TEXT, city TEXT,"
                                                  "onet TEXT, soc5 TEXT, soc2 TEXT);";
char *create_map_onet_soc_table="CREATE TABLE map_onet_soc( onet TEXT, soc5 TEXT);";
char *create_soc_hierarchy_table="CREATE TABLE soc_hierarchy(child TEXT, parent TEXT,"
                                                            "level INT, name TEXT);";
/*open or create sqlite3 database dataline.db
  create three tables
  1.dataline-each row is data from a line in data_in_file
  2.map_onet_soc-mapping of onet to soc5
  3.soc_hierarchy-holds child,parent,level and name information for each soc5 code
*/
void open_sqlite3_db(sqlite3 *dataline_db){
    //open sqlite database
    char *err_message = 0;
    int rc = sqlite3_open("dataline.db", &dataline_db);
    if(rc != SQLITE_OK){//sqlite3 db failed to open
        fprintf(stderr,"[error] dataline_db sqlite3 db failed to open");
        sqlite3_close(dataline_db);
        exit(-3);
    }
    sqlite3_exec(dataline_db,create_dataline_table,0,0, &err_message);
    sqlite3_exec(dataline_db,create_map_onet_soc_table,0,0, &err_message);
    sqlite3_exec(dataline_db,create_soc_hierarchy_table,0,0, &err_message);
    
    fill_map_onet_soc(dataline_db);
}
/*****insert rows into map_onet_soc table from data/map_onet_soc.csv*****/
void fill_map_onet_soc(sqlite3 *dataline_db){
    /****open data/map_onet_soc.csv and check if failed to open, exit if failed****/
    FILE *map_onet_file;
    map_onet_file = fopen("./data/map_onet_soc.csv","r");
    if( map_onet_file == NULL){
        fprintf(stderr, "[error] ./data/map_onet_soc.csv failed to open");
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
    char *err_message = 0;//sqlite3_exec err message parameter
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
            sqlite3_exec(dataline_db,insert_onet_soc5,0,0, &err_message);
            insert_onet_soc5[43] = '\0';//reset insert_onet_soc5 for next instruction
        }
    }
    fclose(map_onet_file);
}
