/*
 * File:   cou.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:24 PM
 */

#pragma once
#include "sys/util.h"

typedef struct {
    size_t db_size;
    char *db_cou_primary;
    char *db_cou_shandow;
    unsigned char *db_cou_curBA;
    unsigned char *db_cou_preBA;
    unsigned char *db_cou_chgBA;
    unsigned char *db_cou_access;
    unsigned char db_cou_lock;

} db_cou_infomation;

int db_cou_init(void *cou_info, size_t db_size);
void *cou_read(size_t index);
int cou_write(size_t index, void *value);
void ckp_cou(int ckp_id, void *cou_info);
void db_cou_destroy(void *cou_info);

