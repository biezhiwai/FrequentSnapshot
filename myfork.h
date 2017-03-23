/*
 * File:   myfork.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:02 PM
 */

#pragma once
#include<pthread.h>


typedef struct {
    size_t db_size;
    char *db_myfork_AS;
    //char *db_myfork_AS_shandow;

    //unsigned char db_myfork_lock;
} db_myfork_infomation;

int db_myfork_init(void *db_myfork_info, size_t db_size);
void* myfork_read(size_t index);
int myfork_write(size_t index, void *value);
void ckp_myfork(int ckp_id, void *db_myfork_info);
void db_myfork_destroy(void *db_myfork_info);

