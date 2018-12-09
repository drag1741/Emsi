/*dataline.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>//allows reading .gz files as stream, must compile with -lz switch

#include "dataline.h"

//initialize a new DataLine struct and return pointer to it
struct DataLine* init_DataLine(){
	//create new DataLine and calloc memory
	struct DataLine *new_data;
	new_data= calloc(1, sizeof(struct DataLine));
	//initialize memory for body, city and title
	new_data-> body_mem_size = 2000;//initialize body text size to 2000 
	new_data-> body = calloc( new_data->body_mem_size + 1, sizeof(char));
	new_data-> city_mem_size = 100;//initialize city name size to 100
	new_data-> city = calloc( new_data-> city_mem_size, sizeof(char));
	new_data-> title_mem_size = 100;//initialize title name size to 100
	new_data-> title = calloc( new_data-> title_mem_size, sizeof(char));
    new_data->date_size = 13; 
    new_data->state_size = 5; 
    new_data->onet_size = 13; 
    new_data->html_removed = 0; 
	return new_data;
}
//fill DataLine struct with data from data_file
//reads one line and parses into struct elements
void read_line(gzFile data_in_file, struct DataLine *data){
    int tag_size = 30;
    char tag[tag_size];
    for( int i = 0 ; i < 7 ; i++ ){//there are 7 different tagged elements per line
        //extract data from data_in_file
        read_next_tag(data_in_file, tag, tag_size);
        if( strncmp(tag,"\"body\"",6) == 0 ){//get text for body
            get_body(data_in_file, data);
        }
        else if( strncmp(tag,"\"title\"",7) == 0 ){//get text for body
            get_title(data_in_file, data);
        }
        else if( strncmp(tag,"\"expired\"",9) == 0 ){//get expired date 
            get_expired_date(data_in_file, data);
        }
        else if( strncmp(tag,"\"posted\"",8) == 0 ){//get expired date 
            get_posted_date(data_in_file, data);
        }
        else if( strncmp(tag,"\"state\"",7) == 0 ){//get state 
            get_state(data_in_file, data);
        }
        else if( strncmp(tag,"\"city\"",6) == 0 ){//get city name 
            get_city(data_in_file, data);
        }
        else if( strncmp(tag,"\"onet\"",6) == 0 ){//get onet code 
            get_onet(data_in_file, data);
        }
    }
}
//print out DataLine struct
void print_data_line(struct DataLine *data){
    fprintf(stdout,"title:%s\n",data->title);
    fprintf(stdout,"posted:%s   expired:%s\n",data->posted_date,data->expired_date);
    fprintf(stdout,"%s, %s\n",data->city,data->state);
    fprintf(stdout,"onet:%s\n",data->onet);
}
//read until next text tag and call corresponding function to fill data struct
void read_next_tag(gzFile in_file, char *tag, int tag_size){
    char in_char;
	while ((in_char = gzgetc( in_file)) != '"');//move forward to tag text
    tag[0]=in_char;//include first '"'
    int i = 1;
	for( ; (in_char = gzgetc( in_file)) != '"' && i < tag_size; i++){
       tag[i] = in_char; 
    }
    tag[i] = in_char;//include last '"' 
    tag[i+1] = '\0';//null terminate
	while ((in_char = gzgetc( in_file)) != '"');//move forward to start of next text 
}
//read body from in_file and place in data_struct->body
void get_body(gzFile in_file, struct DataLine *data){
    char in_char;
	char *body_temp = data->body;
	data->body_size = 0;
    //fill body text
    *body_temp = '"';
    body_temp++;
    data->body_size++;
	while ((in_char = gzgetc( in_file))){
        if(in_char == '"' && *(body_temp-1) != '\\'){//check to see if " is the end of body text
            break;                                   //if " is escaped, \", then it is still in body text
        }
        else{// " was not the end of body text
            //check to see if data excedes buffer size, realloc if true
            if( data->body_size >= data->body_mem_size ){
                data->body = realloc(data->body,2 * data->body_mem_size * sizeof(char));//double memory
                body_temp = data->body + data->body_mem_size;//offset body_temp to new body ptr 
                data->body_mem_size *= 2;//double size of body_mem_size 
            }
            //add in_char to body and increment to next position
            *body_temp = in_char;
            body_temp++;
            data->body_size++;
        }
    }
    //make sure string ends with "\0
    *body_temp = '"';
    body_temp++;
    data->body_size++;
	*body_temp = 0;
}
//read title from in_file and place in data_struct->title
void get_title(gzFile in_file, struct DataLine *data){
    char in_char;
	char *title_temp = data->title;
	data->title_size = 0;
    //fill title text
    *title_temp = '"';
    title_temp++;
    data->title_size++;
	while ((in_char = gzgetc( in_file))){
        if(in_char == '"' && *(title_temp-1) != '\\'){//check to see if " is the end of title text
            break;                                   //if " is escaped, \", then it is still in title text
        }
        else{// " was not the end of title text
            //check to see if data excedes buffer size, realloc if true
            if( data->title_size >= data->title_mem_size ){
                data->title = realloc(data->title,2 * data->title_mem_size * sizeof(char));//double memory
                title_temp = data->title + data->title_mem_size;//offset title_temp to new title ptr 
                data->title_mem_size *= 2;//double size of title_mem_size 
            }
            //add in_char to title and increment to next position
            *title_temp = in_char;
            title_temp++;
            data->title_size++;
        }
    }
    //make sure string ends with "\0
    *title_temp = '"';
    title_temp++;
    data->title_size++;
	*title_temp = 0;
}
//read expired date from in_file and place in data->expired_date
void get_expired_date(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through expired_date[]
    //fill expired_date text
    data->expired_date[i] = '"';
    i++;
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->date_size; i++){
        data->expired_date[i] = in_char;
	}
    data->expired_date[i] = in_char;//add ending '"'
    data->expired_date[i+1] = '\0';//add null terminator
}
//read posted date from in_file and place in data_struct->posted
void get_posted_date(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through posted_date[]
    //fill posted_date text
    data->posted_date[i] = '"';
    i++;
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->date_size; i++){
        data->posted_date[i] = in_char;
	}
    data->posted_date[i] = in_char;//add ending '"'
    data->posted_date[i+1] = '\0';//add null terminator
}
//read state from in_file and place in data_struct->state
void get_state(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through state[]
    //fill state text
    data->state[i] = '"';
    i++;
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->state_size; i++){
        data->state[i] = in_char;
	}
    data->state[i] = in_char;//add ending '"'
    data->state[i+1] = '\0';//add null terminator
}
//read city name from in_file and place in data_struct->city
void get_city(gzFile in_file, struct DataLine *data){
    char in_char;
	char *city_temp = data->city;
	data->city_size = 0;
    //fill city text
    *city_temp = '"';
    city_temp++;
    data->city_size++;
	while ((in_char = gzgetc( in_file)) != '"'){
		if( data->city_size >= data->city_mem_size ){//check to see if data excedes buffer size, realloc if true
			fprintf(stderr, "\n[get_city] data size exceeded data->city_mem_size\n");
			fprintf(stderr, "[get_city] increasing memory size from %d to %d\n\n", data->city_mem_size, data->city_mem_size * 2);
            data->city = realloc(data->city,2 * data->city_mem_size * sizeof(char));//double memory
            city_temp = data->city + data->city_mem_size;//offset city_temp to new city ptr 
            data->city_mem_size *= 2;//double size of city_mem_size 
		}
        *city_temp = in_char;
        city_temp++;
        data->city_size++;
	}
    *city_temp = '"';
    city_temp++;
    data->city_size++;
	*city_temp = 0;//add null terminator
}
//read onet code from in_file and place in data_struct->onet
void get_onet(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through onet[]
    //fill onet text
    data->onet[i] = '"';
	for( i = 1; ((in_char = gzgetc( in_file)) != '"') && (i < data->onet_size); i++){
        data->onet[i] = in_char;
	}
    data->onet[i] = in_char;//add ending '"'
    data->onet[i+1] = '\0';//add null terminator
}
