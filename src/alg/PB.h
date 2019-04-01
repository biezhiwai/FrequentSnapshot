/*
 * File:   mk.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:37 PM
 */
#pragma once

#include<pthread.h>

typedef struct {
    size_t db_size;
    char *db_mk_as1;
    char *db_mk_as2;
    bool *db_mk_ba;
    unsigned char *db_mk_access;
    unsigned char current;
} db_pb_infomation;


int db_pb_init(void *mk_info, size_t db_size);

void *mk_read(size_t index);

int mk_write(size_t index, void *value);

void db_pb_ckp(int ckp_order, void *mk_info);

void db_pb_destroy(void *mk_info);

