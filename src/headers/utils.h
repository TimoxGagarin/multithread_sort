#pragma once
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    double time_mark;
    uint64_t recno;
} index_record_t;

typedef struct
{
    uint64_t count;
    index_record_t *records;
} index_header_t;

typedef struct
{
    index_record_t *begin;
    int block_size;
    int thread_num;
} thread_args_t;

extern int memsize;
extern int blocks_count;
extern int threads_count;
extern pthread_barrier_t barrier;
extern pthread_mutex_t mutex;
extern index_record_t *curr_block_memsize;

int cpu_cores();
int compare(const void *a, const void *b);
void *sorting(void *_args);
void merge(void *_args);
void merge_2blocks(index_record_t *block1_begin, int block_size);