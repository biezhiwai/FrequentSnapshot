/*
 * File:   zigzag.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:02 PM
 */
#pragma once

#include<pthread.h>

typedef struct {
    size_t db_size;
    char *db_zigzag_as0;
    char *db_zigzag_as1;
    unsigned char *db_zigzag_mr;
    unsigned char *db_zigzag_mw;

} db_zigzag_infomation;

int db_zigzag_init(void *cou_info, size_t db_size);

void *zigzag_read(size_t index);

int zigzag_write(size_t index, void *value);

void db_zigzag_ckp(int ckp_order, void *cou_info);

void db_zigzag_destroy(void *cou_info);

