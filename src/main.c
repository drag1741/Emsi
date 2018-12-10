/*main.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>//allows reading .gz files as stream, must compile with -lz switch

#include "../sqlite3/sqlite3.h"//interface to the sqlite database
#include "dataline.h"
#include "sql3functions.h"

FILE *err_log;//error log file declared in sql3functions.h

/*********** main *****************************************************************************/
int main(int argc, char *argv[]){
    /**********open error log file err.log******/
    err_log = fopen("err.log","w"); 
	if ( err_log == NULL ){
		printf("\nerr.log failed to open...exiting program.");
		exit(-1);
	}
    /**********check argc if datafile.gz was passed on cmdline*******/
	if ( argc < 2 ){
		printf("\n[usage] %s <datafile.gz>\n\n", argv[0]);
		fprintf(err_log,"[main] %s <datafile.gz>\n\n", argv[0]);
		exit(-1);
	}
    /**********open data file with file descriptor data_in_file******/
	gzFile data_in_file;
	data_in_file = gzopen(argv[1], "rb");
	if( data_in_file == NULL ){//check for file open error
		fprintf(err_log, "[main]%s failed to open...exiting program\n\n", argv[1]);
		exit(-2);
	}
    /**********open and create sqlite3 db****************************/
    sqlite3 *dataline_db;
    //open sqlite database
    int rc = sqlite3_open("dataline.db", &dataline_db);
    if(rc != SQLITE_OK){//sqlite3 db failed to open
        fprintf(err_log,"[main] dataline_db sqlite3 db failed to open");
        exit(-3);
    }
    open_sqlite3_db(dataline_db);
    /***********initialize DataLine struct and read lines in*********/
	struct DataLine* data = init_DataLine();
    unsigned long html_count = 0;//for summary, keep track of how many lines had html removed
    unsigned long lines_count = 0;//for summary, number of lines read in from file 
    char in_char;//used to look for EOF
    while(gzeof(data_in_file) != 1){//read until end of file
        read_line(data_in_file, data);
        get_soc5(dataline_db, data);//inject soc5 code
        get_soc2(dataline_db, data);//inject soc2 code
        fill_dataline_table(dataline_db, data);
        lines_count++;
        if(lines_count %100 == 0){
            fprintf(stderr,"############################################\n");
            fprintf(stderr,"line:%li\n",lines_count);
            print_data_line(data);
        }
        if(data->html_removed != 0) html_count++;
        if(in_char = gzgetc(data_in_file) != EOF)//look for EOF
            gzungetc(in_char,data_in_file);
    }
    /**********run summary sqlite commands to stdout*****************/
    //lines with html removed count 
    fprintf(stdout,"summary\n");
    fprintf(stdout,"####################################\n");
    fprintf(stdout,"lines read\n");
    fprintf(stdout,"------------------------------------\n");
    fprintf(stdout,"%li\n",lines_count);
    fprintf(stdout,"####################################\n");
    fprintf(stdout,"lines with html removed\n");
    fprintf(stdout,"------------------------------------\n");
    fprintf(stdout,"%li\n",html_count);
    //count of lines grouped by soc2
    fprintf(stdout,"####################################\n");
    fprintf(stdout,"soc2 code: # of lines with soc2 code\n");
    fprintf(stdout,"------------------------------------\n");
    sqlite3_stmt *stmt;
    const char *soc2_count ="SELECT soc2,count(body) FROM dataline GROUP BY soc2 ORDER BY soc2;";
    if(sqlite3_prepare_v2(dataline_db,soc2_count,-1,&stmt,NULL)!= SQLITE_OK){
        fprintf(err_log,"[main] soc2_count failed:%s",sqlite3_errmsg(dataline_db));
    }
    else{
       while(sqlite3_step(stmt) == SQLITE_ROW){
            fprintf(stdout,"%s:%d\n",sqlite3_column_text(stmt,0),sqlite3_column_int(stmt,1));
        } 
    }
    //count of lines posted on 2017-02-01
    fprintf(stdout,"####################################\n");
    fprintf(stdout,"posted date: # of lines \n");
    fprintf(stdout,"------------------------------------\n");
    const char *posted_count ="SELECT posted,count(body) FROM dataline WHERE posted=\"2017-02-01\";";
    if(sqlite3_prepare_v2(dataline_db,posted_count,-1,&stmt,NULL)!= SQLITE_OK){
        fprintf(err_log,"[main] posted_count failed:%s",sqlite3_errmsg(dataline_db));
    }
    else{
       while(sqlite3_step(stmt) == SQLITE_ROW){
            fprintf(stdout,"%s:%d\n","2017-02-01",sqlite3_column_int(stmt,1));
        } 
    }
    sqlite3_finalize(stmt);

    /**********free memory, close files and return*******************/
	delete_data(data);
    free(data);
	gzclose(data_in_file);
    fclose(err_log);
    sqlite3_close(dataline_db);
	return 0;
}
