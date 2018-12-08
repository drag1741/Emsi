/*main.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>//allows reading .gz files as stream, must compile with -lz switch


struct DataLine{//store data from processed line
    char *body;//body text
	unsigned int body_mem_size;//size of memory allocated to body
    unsigned long body_size;//number of chars in body
    char posted_date[11];// MM/DD/YYYY
    char expired_date[11];// MM/DD/YYYY
    char state[3];
    char *city;
	unsigned int city_mem_size;//size of memory allocated to city 
	unsigned int city_size;
    char soc5[11];//the soc5 string has a preset size of 10
    char soc2[10];
};

/*********** function declarations  ***********************************************************/
//read from in_file into in_buffer until \n
void gzgetline(gzFile in_file, struct DataLine *data_struct);
//initialize a new DataLine struct and return pointer to it
struct DataLine* init_data_line();

/*********** main *****************************************************************************/
int main(int argc, char *argv[]){
//check for cmdline arguments, input file must be argv[1] 
	if ( argc < 2 ){
		printf("\n[usage] %s <datafile.gz>\n\n", argv[0]);
		exit(-1);
	}
//open data file with file descriptor data_in_file
	gzFile data_in_file;
	data_in_file = gzopen(argv[1], "rb");
	if( data_in_file == NULL ){//check for file open error
		fprintf( stderr, "\n[error] %s failed to open...exiting program\n\n", argv[1]);
		exit(-2);
	}
//intialize DataLine struct
	struct DataLine* data = init_data_line();
//read data from data_in_file
	for( int i = 0 ; i < 5 ; i++ ){
		//extract data from data_in_file
		gzgetline(data_in_file, data);
		fprintf(stdout,"%s\n", data->body);
		fprintf(stdout,"\ndata->body_size: %d\n\n",data->body_size);
		
		//call sqlite db
	}
//free memory close file and return
	free(data);
	gzclose(data_in_file);
	return 0;
}

/*********** function definitions  ************************************************************/
void gzgetline(gzFile in_file, struct DataLine *data){
	char *buffer_temp = data-> body;
	data->body_size = 0;
	// enter chars from line of data from in_file until \n
	for(; (*buffer_temp = gzgetc( in_file)) != '\n' ; buffer_temp++, data->body_size++){
		if( data->body_size >= data->body_mem_size ){//check to see if data excedes buffer size, realloc if true
			fprintf(stderr, "\n[gzgetline] data size exceeded data->body_mem_size\n");
			fprintf(stderr, "[gzgetline] increasing memory size from %d to %d\n\n", data->body_mem_size, data->body_mem_size * 2);
            data->body = realloc(data->body,2 * data->body_mem_size * sizeof(char));//double memory
            buffer_temp = data->body + data->body_mem_size;//offset buffer_temp to new body ptr 
            data->body_mem_size *= 2;//double size of in_buffer
		}
	}
	*buffer_temp = 0;//add null terminator
}

//initialize a new DataLine struct and return pointer to it
struct DataLine* init_data_line(){
	//create new DataLine and calloc memory
	struct DataLine *new_data_line;
	new_data_line = calloc(1, sizeof(struct DataLine));
	//initialize memory for body and city
	new_data_line-> body_mem_size = 2000;//initialize body text size to 2000 
	new_data_line -> body = calloc( new_data_line->body_mem_size + 1, sizeof(char));
	new_data_line-> city_mem_size = 50;//initialize city size name is 50
	new_data_line -> city = calloc( new_data_line-> city_mem_size, sizeof(char));

	return new_data_line;
}
