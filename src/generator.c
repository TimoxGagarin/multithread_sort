#include "headers/utils.h"

int memsize;
int blocks_count;
int threads_count;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
index_record_t *curr_block_memsize;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        puts("Invalid usage. Correct program usage: ./generator <records_count> <filename>");
        exit(EXIT_FAILURE);
    }

    index_header_t header;
    header.count = atoi(argv[1]);
    if (header.count % 256 != 0)
    {
        printf("Invalid number of records. Must be a multiple of 256\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    header.records = (index_record_t *)malloc(header.count * sizeof(index_record_t));
    for (uint64_t i = 0; i < header.count; i++)
    {
        header.records[i].recno = i + 1;
        header.records[i].time_mark = 15020 + rand() % (60420 - 15020 + 1);
        header.records[i].time_mark += 1.0f * ((rand() % 24) * 3600 + (rand() % 60) * 61) / (24 * 3600);
    }

    FILE *file = fopen(argv[2], "wb");
    if (!file)
        printf("File creating error: %s", argv[2]);
    else
    {
        fwrite(&header.count, sizeof(uint64_t), 1, file);
        fwrite(header.records, sizeof(index_record_t), header.count, file);
    }

    fclose(file);
    free(header.records);
    exit(EXIT_SUCCESS);
}