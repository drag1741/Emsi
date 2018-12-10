/*dataline.c
*
* Author: Jay Drage
* Purpose: Emsi technical interview question
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>//allows reading .gz files as stream, must compile with -lz switch

#include "../sqlite3/sqlite3.h"
#include "dataline.h"
#include "sql3functions.h"

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
    int newline_flag = 0;
    int body_flag = 1;
    int title_flag = 1;
    int expired_flag = 1;
    int posted_flag = 1;
    int state_flag = 1;
    int city_flag = 1;
    int onet_flag = 1;
    /***end loop if \n is encountered, isolates any corruption of data to a single line*/
    while( newline_flag !=1 ){
        newline_flag = read_next_tag(data_in_file, tag, tag_size);//read data tag
        //extract data from data_in_file
        if( strncmp(tag,"\"body\"",6) == 0 && body_flag){//get text for body
            newline_flag = get_body(data_in_file, data);
            body_flag = 0;
        }
        else if( strncmp(tag,"\"title\"",7) == 0 && title_flag){//get text for body
            newline_flag = get_title(data_in_file, data);
            title_flag = 0;
        }
        else if( strncmp(tag,"\"expired\"",9) == 0 && expired_flag){//get expired date 
            newline_flag = get_expired_date(data_in_file, data);
            expired_flag = 0;
        }
        else if( strncmp(tag,"\"posted\"",8) == 0 && posted_flag){//get expired date 
            newline_flag = get_posted_date(data_in_file, data);
            posted_flag = 0;
        }
        else if( strncmp(tag,"\"state\"",7) == 0 && state_flag){//get state 
            newline_flag = get_state(data_in_file, data);
            state_flag = 0;
        }
        else if( strncmp(tag,"\"city\"",6) == 0 && city_flag){//get city name 
            newline_flag = get_city(data_in_file, data);
            city_flag = 0;
        }
        else if( strncmp(tag,"\"onet\"",6) == 0 && onet_flag){//get onet code 
            newline_flag = get_onet(data_in_file, data);
            onet_flag = 0;
        }
        else if (newline_flag != 1){
            while(gzgetc(data_in_file) != '\n');//move stream to start of next line
            newline_flag = 1;
        }
    }
}
//print out DataLine struct
void print_data_line(struct DataLine *data){
    fprintf(stderr,"title:%s\n",data->title);
    fprintf(stderr,"posted:%s   expired:%s\n",data->posted_date,data->expired_date);
    fprintf(stderr,"%s, %s\n",data->city,data->state);
    fprintf(stderr,"onet:%s\n",data->onet);
    fprintf(stderr,"soc5:%s\n",data->soc5);
    fprintf(stderr,"soc2:%s\n",data->soc2);
}
//read until next text tag and call corresponding function to fill data struct
int read_next_tag(gzFile in_file, char *tag, int tag_size){
    char in_char;
	while ((in_char = gzgetc( in_file)) != '"'){//move forward to tag text
        if(in_char == '\n'){
            return 1;//safety against corrupted line of data
        }
    }
    tag[0]=in_char;//include first '"'
    int i = 1;
	for( ; (in_char = gzgetc( in_file)) != '"' && i < tag_size; i++){
       tag[i] = in_char; 
    }
    tag[i] = in_char;//include last '"' 
    tag[i+1] = '\0';//null terminate
	while ((in_char = gzgetc( in_file)) != '"');//move forward to start of next text 
    return 0;
}
//read body from in_file and place in data_struct->body
int get_body(gzFile in_file, struct DataLine *data){
    char in_char;
	char *body_temp = data->body;
    int html_flag = 0;//set to 1 if inside an html tag
	data->body_size = 0;
    //fill body text
	while ((in_char = gzgetc( in_file))){
        if(in_char == '\n') return 1;//safety against corrupted line of data
        if(in_char == '<'){ 
            html_flag = 1;//start of html tag
            data->html_removed++;
        }
        if(html_flag != 1){//do not save in_char if inside an html tag
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
        if(in_char == '>') html_flag = 0;//end of html tag
    }
    //make sure string ends with "\0
	*body_temp = 0;
    data->body_size++;
    return 0;
}
//read title from in_file and place in data_struct->title
int get_title(gzFile in_file, struct DataLine *data){
    char in_char;
	char *title_temp = data->title;
	data->title_size = 0;
    //fill title text
	while ((in_char = gzgetc( in_file))){
        if(in_char == '\n') return 1;//safety against corrupted line of data
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
    data->title_size++;
	*title_temp = 0;
    return 0;
}
//read expired date from in_file and place in data->expired_date
int get_expired_date(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through expired_date[]
    //fill expired_date text
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->date_size; i++){
        if(in_char == '\n') return 1;//safety against corrupted line of data
        data->expired_date[i] = in_char;
	}
    data->expired_date[i] = '\0';//add null terminator
    return 0;
}
//read posted date from in_file and place in data_struct->posted
int get_posted_date(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through posted_date[]
    //fill posted_date text
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->date_size; i++){
        if(in_char == '\n') return 1;//safety against corrupted line of data
        data->posted_date[i] = in_char;
	}
    data->posted_date[i] = '\0';//add null terminator
    return 0;
}
//read state from in_file and place in data_struct->state
int get_state(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through state[]
    //fill state text
	for(; (in_char = gzgetc( in_file)) != '"' && i < data->state_size; i++){
        if(in_char == '\n') return 1;//safety against corrupted line of data
        data->state[i] = in_char;
	}
    data->state[i] = '\0';//add NULL terminator
    return 0;
}
//read city name from in_file and place in data_struct->city
int get_city(gzFile in_file, struct DataLine *data){
    char in_char;
	char *city_temp = data->city;
	data->city_size = 0;
    //fill city text
	while ((in_char = gzgetc( in_file)) != '"'){
        if(in_char == '\n') return 1;//safety against corrupted line of data
		if( data->city_size >= data->city_mem_size ){//check to see if data excedes buffer size, realloc if true
			fprintf(err_log, "[get_city] data size exceeded data->city_mem_size\n");
			fprintf(err_log, "[get_city] increasing memory size from %d to %d\n\n", data->city_mem_size, data->city_mem_size * 2);
            data->city = realloc(data->city,2 * data->city_mem_size * sizeof(char));//double memory
            city_temp = data->city + data->city_mem_size;//offset city_temp to new city ptr 
            data->city_mem_size *= 2;//double size of city_mem_size 
		}
        *city_temp = in_char;
        city_temp++;
        data->city_size++;
	}
    *city_temp = '\0';//add null terminator
    data->city_size++;
    return 0;
}
//read onet code from in_file and place in data_struct->onet
int get_onet(gzFile in_file, struct DataLine *data){
    char in_char;
    int i = 0;//index through onet[]
    //fill onet text
	for( ; ((in_char = gzgetc( in_file)) != '"') && (i < data->onet_size); i++){
        if(in_char == '\n') return 1;//safety against corrupted line of data
        data->onet[i] = in_char;
	}
    data->onet[i] = '\0';//add ending '"'
    return 0;
}
//get soc5 code from map_onet_soc5 table in dataline_db
void get_soc5(sqlite3 *dataline_db, struct DataLine *data){
    /***build sqlite3 command***/
    sqlite3_stmt *stmt;
    const char *sql = "SELECT soc5 FROM map_onet_soc WHERE onet=(?);";
    //"SELECT soc5 FROM map_onet_soc5 WHERE onet=data->onet";
    if(sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(err_log,"[get_soc5]sqlite3_prepare_v2 failed: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 1, data->onet,-1, NULL) != SQLITE_OK){
        fprintf(err_log,"[get_soc5]sqlite3_bind_text failed: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_step(stmt) != SQLITE_ROW){//returns SQLITE_ROW
        fprintf(err_log,"[get_soc5]sqlite3_step failed: %s\n",sqlite3_errmsg(dataline_db));
        strncpy(data->soc5,"no code",7);
    }
    else{
        const char *soc5 = sqlite3_column_text(stmt,0);//returns soc5 code 
        strncpy(data->soc5,soc5,7);
    }
    sqlite3_finalize(stmt);
}
//get soc2 code from soc_hierarchy table in dataline_db
void get_soc2(sqlite3 *dataline_db, struct DataLine *data){
    /***build sqlite3 command***/
    sqlite3_stmt *stmt;
    const char *sql = "SELECT parent FROM soc_hierarchy WHERE child=(?);";
    //"SELECT soc5 FROM map_onet_soc5 WHERE onet=data->onet";
    if(sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL) != SQLITE_OK){
       fprintf(err_log,"[get_soc2]sqlite3_prepare_v2 failed: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 1, data->soc5,-1, NULL) != SQLITE_OK){
       fprintf(err_log,"[get_soc2]sqlite3_bind_text failed: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_step(stmt) != SQLITE_ROW){//returns SQLITE_ROW
       fprintf(err_log,"[get_soc2]step:%s\tonet:%s\tsoc5:%s\n",sqlite3_errmsg(dataline_db),data->onet,data->soc5);
       strncpy(data->soc2,"no code",7);
    }
    else{
        const char *soc2 = sqlite3_column_text(stmt,0);//returns soc2 code 
        strncpy(data->soc2,soc2,7);
    }
    sqlite3_finalize(stmt);
}
//fill dataline table with data, each data struct is a row in db
void fill_dataline_table(sqlite3 *dataline_db, struct DataLine *data){
    //
    /***build sqlite3 command***/
    sqlite3_stmt *stmt;
    const char *sql ="INSERT INTO dataline(body,title,expired,posted,state,city,onet,soc5,soc2)"
                                            "VALUES(?,?,?,?,?,?,?,?,?);";  
    //bind items from data to stmt
    if(sqlite3_prepare_v2(dataline_db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(err_log,"[fill_dataline]sqlite3_prepare_v2 failed: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 1, data->body,-1, NULL) != SQLITE_OK){//body
        fprintf(err_log,"[fill_dataline]sqlite3_bind_text body: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 2, data->title,-1, NULL) != SQLITE_OK){//title
        fprintf(err_log,"[fill_dataline]sqlite3_bind_text title: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 3, data->expired_date,-1, NULL) != SQLITE_OK){//expired_date
        fprintf(err_log,"[fill_dataline]sqlite3_bind_date expired: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 4, data->posted_date,-1, NULL) != SQLITE_OK){//posted_date
        fprintf(err_log,"[fill_dataline]sqlite3_bind_date posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 5, data->state,-1, NULL) != SQLITE_OK){//state
        fprintf(err_log,"[fill_dataline]sqlite3_bind_state posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 6, data->city,-1, NULL) != SQLITE_OK){//city
        fprintf(err_log,"[fill_dataline]sqlite3_bind_city posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 7, data->onet,-1, NULL) != SQLITE_OK){//onet
        fprintf(err_log,"[fill_dataline]sqlite3_bind_onet posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 8, data->soc5,-1, NULL) != SQLITE_OK){//soc5
        fprintf(err_log,"[fill_dataline]sqlite3_bind_soc5 posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    if(sqlite3_bind_text(stmt, 9, data->soc2,-1, NULL) != SQLITE_OK){//soc2
        fprintf(err_log,"[fill_dataline]sqlite3_bind_soc2 posted: %s\n",sqlite3_errmsg(dataline_db));
    }
    sqlite3_step(stmt);//send command
    sqlite3_finalize(stmt);//destroy command
}
//frees all memory in data struct
void delete_data(struct DataLine *data){
    free(data->body);
    free(data->city);
    free(data->title);
}
