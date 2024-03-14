/*
 * File:   myfork.h
 * Author: mk
 *
 * Created on September 20, 2015, 8:02 PM
 */

#pragma once

#include<pthread.h>
#include "util.h"

// typedef struct node{
//     integer index;
//     char* buf;
//     struct node* next;
//     struct node* prev;
// }node;

// typedef struct{
//     int size;
//     node* head;
//     node* tail;
// }DoubleList;

// typedef struct{
//     int capacity;
//     DoubleList* list;
//     node** map;
// }LRUCache;


// DoubleList* createDoubleList();

// void addLast(DoubleList* list, node* x);

// void removeX(DoubleList* list, node* x);

// node* removeFirst(DoubleList* list);

// int getSize(DoubleList* list);

// LRUCache* createLRUCache(int capacity);

// void makeRecently(LRUCache* cache, integer index);

// void addRecently(LRUCache* cache, integer index, char* buf);

// void deleteKey(LRUCache* cache, integer index);

// node* removeLeastRecently(LRUCache* cache);

// char* get(LRUCache* cache, integer index);

// node* put(LRUCache* cache, integer index, char* buf);


typedef struct {
    integer db_size;
    char *db_small_page;
    char *db_huge_page;
    integer small_page_size;
    integer huge_page_size;
    float huge_page_ratio;
    // LRUCache *db_small_page;
    // integer *addr_map;
    // int *vis_count;
} db_myfork_lru_infomation;


int db_myfork_lru_init(void *db_myfork_lru_info, size_t db_size);

void *myfork_lru_read(size_t index);

int myfork_lru_write(size_t index, void *value);

void ckp_myfork_lru(int ckp_id, void *db_myfork_lru_info);

void db_myfork_lru_destroy(void *db_myfork_lru_info);

