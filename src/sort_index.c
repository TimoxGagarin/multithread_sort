#include "headers/utils.h"

int memsize;
int blocks_count;
int threads_count;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
index_record_t *curr_block_memsize;

int main(int argc, char *argv[])
{
    int cpu_cores_count = cpu_cores();
    if (argc != 5)
    {
        puts("Invalid usage. Correct program usage: ./sort_index <memsize> <blocks> <threads> <filename>");
        exit(EXIT_FAILURE);
    }

    memsize = atoi(argv[1]);
    blocks_count = atoi(argv[2]);
    threads_count = atoi(argv[3]) - 1;

    if (memsize % getpagesize() != 0)
    {
        printf("Invalid memsize. Must be a multiple of 4096\n");
        exit(EXIT_FAILURE);
    }
    if (threads_count < cpu_cores_count || threads_count > 8000)
    {
        printf("Invalid threads count. Must be between %i and 8000\n", cpu_cores_count);
        exit(EXIT_FAILURE);
    }
    if (blocks_count % 2 || blocks_count < threads_count)
    {
        printf("Invalid blocks count. Must be even and greater than threads count\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[4];
    int block_size = memsize / blocks_count;

    FILE *file = fopen(filename, "rb+");
    if (file == NULL)
    {
        fprintf(stderr, "Open file error\n");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    int fd = fileno(file);

    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, threads_count);

    void *begin = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0) + sizeof(uint64_t);
    curr_block_memsize = (index_record_t *)begin;
    pthread_t children_threads[threads_count - 1];

    for (int i = 1; i < threads_count; i++)
    {
        thread_args_t *args = (thread_args_t *)malloc(sizeof(thread_args_t));
        args->block_size = block_size;
        args->begin = (index_record_t *)begin;
        args->thread_num = i;
        pthread_create(children_threads + i - 1, NULL, sorting, args);
    }
    thread_args_t *args = (thread_args_t *)malloc(sizeof(thread_args_t));
    args->block_size = block_size;
    args->begin = (index_record_t *)begin;
    args->thread_num = 0;
    sorting(args);

    for (int i = 1; i < threads_count; i++)
        pthread_join(children_threads[i - 1], NULL);
    munmap(begin - sizeof(uint64_t), file_size);
    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);
    fclose(file);
    exit(EXIT_SUCCESS);
}