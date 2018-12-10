/*dataline.h
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#ifndef DATALINE_H
#define DATALINE_H

#include <zlib.h>

struct DataLine{//store data from processed line
    char *body;//body text
	unsigned int body_mem_size;//size of memory allocated to body
    unsigned long body_size;//size of body string
    char posted_date[13]; // "YYYY-MM-DD"
    char expired_date[13];// "YYYY-MM-DD"
    unsigned int date_size;//set to 13 in init_DataLine()
    char state[5];// "ST"
    unsigned int state_size;//set to 5 in init_DataLine()
    char *city;//name of city
	unsigned int city_mem_size;//size of memory allocated to city 
	unsigned int city_size;//size of city string
    char *title;//title of position
	unsigned int title_mem_size;//size of memory allocated to title 
	unsigned int title_size;//size of title string
    char onet[13];//the onet string has a preset size of 13
    unsigned int onet_size;//set to 11 in init_DataLine()
    char soc5[11];//the soc5 string has a preset size of 10
    char soc2[10];
    unsigned int html_removed;//number of html tags removed
};

/*********** function declarations ************************************************************/
//initialize a new DataLine struct and return pointer to it
struct DataLine* init_DataLine();
//fill DataLine struct with data from data_file
//reads one line and parses into struct elements
void read_line(gzFile data_in_file, struct DataLine *data);
//print out DataLine struct
void print_data_line(struct DataLine *data);
//read until next text tag and call corresponding function to fill data struct
int read_next_tag(gzFile in_file, char *tag, int tag_size);
//read body from in_file and place in data_struct->body
int get_body(gzFile in_file, struct DataLine *data);
//read title from in_file and place in data_struct->title
int get_title(gzFile in_file, struct DataLine *data);
//read expired date from in_file and place in data_struct->expired
int get_expired_date(gzFile in_file, struct DataLine *data);
//read posted date from in_file and place in data_struct->posted
int get_posted_date(gzFile in_file, struct DataLine *data);
//read state from in_file and place in data_struct->state
int get_state(gzFile in_file, struct DataLine *data);
//read city name from in_file and place in data_struct->city
int get_city(gzFile in_file, struct DataLine *data);
//read onet code from in_file and place in data_struct->onet
int get_onet(gzFile in_file, struct DataLine *data);
//get soc5 code from map_onet_soc5 table in dataline_db
void get_soc5(sqlite3 *dataline_db, struct DataLine *data);
//get soc2 code from soc_hierarchy table in dataline_db
void get_soc2(sqlite3 *dataline_db, struct DataLine *data);
//fill dataline table with data, each data struct is a row in db
void fill_dataline_table(sqlite3 *dataline_db, struct DataLine *data);
//frees all memory in data struct
void delete_data(struct DataLine *data);

#endif
