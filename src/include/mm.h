/*
 * File:   mk.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:37 PM
 */
#pragma once

#include<pthread.h>
#include "util.h"
#include <sys/mman.h>

typedef struct 
{
    integer *base;
    integer *top;
    bool *vis;
}Stack;

typedef struct {
    integer db_size;
    char *db_mm_as1;
    Stack *to_dump;
    Stack *update;
} db_mm_infomation;


int db_mm_init(void *mm_info, size_t db_size);

void *mm_read(size_t index);

int mm_write(size_t index, void *value);

void db_mm_ckp(int ckp_order, void *mk_info);

void db_mm_destroy(void *mk_info);

