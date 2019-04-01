/*
 * File:   LL.h
 * Author: mk
 *
 * Created on October 21, 2015, 9:31 AM
 */

#pragma once

#include<pthread.h>

typedef struct {
    size_t db_size;
    char *db_ll_as0;
    char *db_ll_as1;
    char *db_ll_prev;
    bool *db_ll_as0_ba;
    bool *db_ll_as1_ba;
    bool *db_ll_mr_ba;
    int current;

} db_hg_infomation;

int db_hg_init(void *ll_info, size_t db_size);

void *ll_read(size_t index);

int ll_write(size_t index, void *value);

void db_hg_ckp(int ckp_id, void *ll_info);

void db_hg_destroy(void *ll_info);

