#include "headers/utils.h"
#include <dirent.h>

int compare(const void *a, const void *b)
{
    return (double)((index_record_t *)a)->time_mark - (double)((index_record_t *)b)->time_mark;
}

int cpu_cores()
{
    FILE *file;
    char line[1024];
    int cpu_count = 0;

    file = fopen("/proc/cpuinfo", "r");
    if (file == NULL)
    {
        perror("fopen");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
        if (!strncmp(line, "processor", 9))
            cpu_count++;

    fclose(file);
    return cpu_count;
}

void *sorting(void *_args)
{
    thread_args_t *args = (thread_args_t *)_args;
    pthread_barrier_wait(&barrier);

    while (curr_block_memsize < args->begin + memsize)
    {
        pthread_mutex_lock(&mutex);
        if (curr_block_memsize < args->begin + memsize)
        {
            index_record_t *temp = curr_block_memsize;
            curr_block_memsize += args->block_size;
            pthread_mutex_unlock(&mutex);
            qsort(temp, args->block_size, sizeof(index_record_t), compare);
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            pthread_barrier_wait(&barrier);
            break;
        }
    }
    merge(args);
}

void merge(void *_args)
{
    thread_args_t *args = (thread_args_t *)_args;
    int mergeStep = 2;
    while (mergeStep <= blocks_count)
    {
        pthread_barrier_wait(&barrier);
        curr_block_memsize = args->begin;
        while (curr_block_memsize < args->begin + memsize)
        {
            pthread_mutex_lock(&mutex);
            if (curr_block_memsize < args->begin + memsize)
            {
                index_record_t *temp = curr_block_memsize;
                curr_block_memsize += mergeStep * args->block_size;
                pthread_mutex_unlock(&mutex);

                int block_size = (mergeStep / 2) * args->block_size;
                merge_2blocks(temp, block_size);
            }
            else
            {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
        mergeStep *= 2;
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
}

void merge_2blocks(index_record_t *block1_begin, int block_size)
{
    index_record_t *left = (index_record_t *)malloc(block_size * sizeof(index_record_t));
    index_record_t *right = (index_record_t *)malloc(block_size * sizeof(index_record_t));
    memcpy(left, block1_begin, block_size * sizeof(index_record_t));
    memcpy(right, block1_begin + block_size, block_size * sizeof(index_record_t));
    int leftIdx = 0;
    int rightIdx = 0;
    int idx = 0;

    while (leftIdx < block_size && rightIdx < block_size)
    {
        if (left[leftIdx].time_mark > right[rightIdx].time_mark)
        {
            block1_begin[idx].time_mark = right[rightIdx].time_mark;
            block1_begin[idx++].recno = right[rightIdx++].recno;
        }
        else
        {
            block1_begin[idx].time_mark = left[leftIdx].time_mark;
            block1_begin[idx++].recno = left[leftIdx++].recno;
        }
    }

    while (leftIdx < block_size)
    {
        block1_begin[idx].time_mark = left[leftIdx].time_mark;
        block1_begin[idx++].recno = left[leftIdx++].recno;
    }

    while (rightIdx < block_size)
    {
        block1_begin[idx].time_mark = right[rightIdx].time_mark;
        block1_begin[idx++].recno = right[rightIdx++].recno;
    }

    free(left);
    free(right);
}