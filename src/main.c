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

/*********** main *****************************************************************************/
int main(int argc, char *argv[]){
    /**********check argc if datafile.gz was passed on cmdline*******/
	if ( argc < 2 ){
		printf("\n[usage] %s <datafile.gz>\n\n", argv[0]);
		exit(-1);
	}
    /**********open data file with file descriptor data_in_file******/
	gzFile data_in_file;
	data_in_file = gzopen(argv[1], "rb");
	if( data_in_file == NULL ){//check for file open error
		fprintf( stderr, "\n[error] %s failed to open...exiting program\n\n", argv[1]);
		exit(-2);
	}
    /**********open and create sqlite3 db****************************/
    sqlite3 *sqlite3_db;
    open_sqlite3_db(sqlite3_db);
    /***********initialize DataLine struct and read lines in*********/
	struct DataLine* data = init_DataLine();
    for( int j = 0; j < 40000 ; j++){
        read_line(data_in_file, data);
        while(gzgetc(data_in_file) != '\n');//move stream to start of next line
    }
    /**********free memory, close files and return*******************/
	free(data);
	gzclose(data_in_file);
    sqlite3_close(sqlite3_db);
	return 0;
}
