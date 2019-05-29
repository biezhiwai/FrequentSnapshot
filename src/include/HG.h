/*
 * File:   LL.h
 * Author: mk
 *
 * Created on October 21, 2015, 9:31 AM
 */

#pragma once

#include<pthread.h>
#include "util.h"

typedef struct {
    integer db_size;
    char *db_hg_as0;
    char *db_hg_as1;
    char *db_hg_prev;
    unsigned char *db_hg_as0_ba;
    unsigned char *db_hg_as1_ba;
    unsigned char *db_hg_mr_ba;
    int current;

} db_hg_infomation;

int db_hg_init(void *ll_info, size_t db_size);

void *hg_read(size_t index);

int hg_write(size_t index, void *value);

void db_hg_ckp(int ckp_id, void *ll_info);

void db_hg_destroy(void *ll_info);

