/*
 * File:   myfork.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:02 PM
 */

#pragma once

#include<pthread.h>
#include "util.h"

typedef struct {
    integer db_size;
    char *db_small_page;
    char *db_huge_page;
    integer small_page_size;
    integer huge_page_size;
    float huge_page_ratio;
} db_myfork_hotcold_infomation;


int db_myfork_hotcold_init(void *db_myfork_hotcold_info, size_t db_size);

void *myfork_hotcold_read(size_t index);

int myfork_hotcold_write(size_t index, void *value);

void ckp_myfork_hotcold(int ckp_id, void *db_myfork_hotcold_info);

void db_myfork_hotcold_destroy(void *db_myfork_hotcold_info);

