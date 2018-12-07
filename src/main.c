/*main.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>//allows reading .gz files as stream, must compile with -lz switch


int in_buffer_size = 2000;//initial size of in_buffer, used to read chars from data_in_file

struct DataLine{//store data from processed line
    char *body;//body text
    unsigned long body_size;//number of chars in body
    unsigned int posted_date[8];// MMDDYYYY
    unsigned int expired_date[8];// MMDDYYYY
    char state[3];
    char *city;
    char soc5[11];//the soc5 string has a preset size of 10
    char soc2[10];
};

int gzgetline(gzFile in_file, char **in_buffer);//read from in_file into in_buffer until \n

int main(int argc, char *argv[]){
	//check for cmdline arguments
	if ( argc < 2 ){
		printf("[usage] %s <datafile.gz>\n", argv[0]);
		exit(-1);
	}
	//open data file with file descriptor data_in_file
	gzFile data_in_file;
	data_in_file = gzopen(argv[1], "rb");
	if( data_in_file == NULL ){//check for file open error
		fprintf( stderr, "file failed to open...exiting");
		exit(-1);
	}

	//read data from data_in_file
	char *in_buffer;
	int in_buffer_len = 0;
	in_buffer = calloc( in_buffer_size + 1, sizeof(char));
	in_buffer_len = gzgetline(data_in_file, &in_buffer);
	fprintf(stdout,"%s\n", in_buffer);
	//fwrite(in_buffer,sizeof(char), in_buffer_size, stdout);	
	fprintf(stdout,"\nin_buffer_len: %d\n",in_buffer_len);

    fprintf( stdout, "\n");

	in_buffer_len = gzgetline(data_in_file, &in_buffer);
	fprintf(stdout,"%s\n", in_buffer);
	fprintf(stdout,"\nin_buffer_len: %d\n",in_buffer_len);

    fprintf( stdout, "\n");

	in_buffer_len = gzgetline(data_in_file, &in_buffer);
	fprintf(stdout,"%s\n", in_buffer);
	fprintf(stdout,"\nin_buffer_len: %d\n",in_buffer_len);

    free(in_buffer);
	gzclose(data_in_file);
	return 0;
}

int gzgetline(gzFile in_file, char **in_buffer){
	int length = 0;//number of bytes read
	char *buffer_temp = *in_buffer;
	// enter line of data
	for(; (*buffer_temp = gzgetc( in_file)) != '\n' ; buffer_temp++, length++){
		if( length >= in_buffer_size ){//check to see if data excedes buffer size, realloc if true
			fprintf(stderr, "\n[gzgetline] data size exceeded in_buffer_size\n");
			fprintf(stderr, "[gzgetline] increasing memory size from %d to %d\n\n", in_buffer_size, in_buffer_size * 2);
            *in_buffer = realloc(*in_buffer,2 * in_buffer_size * sizeof(**in_buffer));//double memory
            buffer_temp = *in_buffer + in_buffer_size;//offset buffer_temp to new *in_buffer
            in_buffer_size *= 2;//double size of in_buffer
		}
	}
	return length;
}
