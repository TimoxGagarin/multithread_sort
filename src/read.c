#include "headers/utils.h"

int memsize;
int blocks_count;
int threads_count;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
index_record_t *curr_block_memsize;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid usage. Correct program usage: ./read <input_filename>\n");
        exit(EXIT_FAILURE);
    }

    FILE *input = fopen(argv[1], "rb");
    if (input == NULL)
    {
        fprintf(stderr, "File openning error: %s", argv[2]);
        exit(EXIT_FAILURE);
    }

    index_header_t *data = (index_header_t *)malloc(sizeof(index_header_t));
    if (!fread(&data->count, sizeof(uint64_t), 1, input))
    {
        fprintf(stderr, "Failure reading of records count\n");
        fclose(input);
        exit(EXIT_FAILURE);
    }

    data->records = (index_record_t *)malloc(data->count * sizeof(index_record_t));
    if (!fread(data->records, sizeof(index_record_t), data->count, input))
    {
        fprintf(stderr, "Failure reading array of records\n");
        fclose(input);
        exit(EXIT_FAILURE);
    }
    fclose(input);

    for (int i = 0; i < data->count; i++)
        printf("%lu %lf\n", data->records[i].recno, data->records[i].time_mark);

    free(data->records);
    free(data);
    exit(EXIT_SUCCESS);
}