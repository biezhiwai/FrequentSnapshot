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

    void** cold_update;
    integer *base;
    int size;
} db_myfork_hotcold_cou_infomation;


int db_myfork_hotcold_cou_init(void *db_myfork_hotcold_cou_info, size_t db_size);

void *myfork_hotcold_cou_read(size_t index);

int myfork_hotcold_cou_write(size_t index, void *value);

void ckp_myfork_hotcold_cou(int ckp_id, void *db_myfork_hotcold_cou_info);

void db_myfork_hotcold_cou_destroy(void *db_myfork_hotcold_cou_info);

