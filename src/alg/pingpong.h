/*
 * File:   pingpong.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:34 PM
 */

#pragma once

#include<pthread.h>

typedef struct {
    size_t db_size;
    char *db_pp_as;
    char *db_pp_as_odd;
    char *db_pp_as_even;
    char *db_pp_as_previous;
    bool *db_pp_odd_ba;
    bool *db_pp_even_ba;
    int current;
} db_pingpong_infomation;

int db_pingpong_init(void *pp_info, size_t db_size);

void *pingpong_read(size_t index);

int pingpong_write(size_t index, void *value);

void db_pingpong_ckp(int ckp_order, void *pp_info);

void db_pingpong_destroy(void *pp_info);

