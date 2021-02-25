
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "eocdr.h"

#define PERROR_IF(cond, msg) if (cond) { perror(msg); exit(1); }

static void *xmalloc(size_t size)
{
        void *ptr = malloc(size);
        PERROR_IF(ptr == NULL, "malloc");
        return ptr;
}

static void *xrealloc(void *ptr, size_t size)
{
        ptr = realloc(ptr, size);
        PERROR_IF(ptr == NULL, "realloc");
        return ptr;
}

static uint8_t *read_file(const char *filename, size_t *file_sz)
{
        FILE *f;
        uint8_t *buf;
        size_t buf_cap;

        f = fopen(filename, "rb");
        PERROR_IF(f == NULL, "fopen");

        buf_cap = 4096;
        buf = xmalloc(buf_cap);

        *file_sz = 0;
        while (feof(f) == 0) {
                if (buf_cap - *file_sz == 0) {
                        buf_cap *= 2;
                        buf = xrealloc(buf, buf_cap);
                }

                *file_sz += fread(&buf[*file_sz], 1, buf_cap - *file_sz, f);
                PERROR_IF(ferror(f), "fread");
        }

        PERROR_IF(fclose(f) != 0, "fclose");
        return buf;
}

int main(int argc, const char * argv[]) {
    size_t file_size;
    uint8_t *file_data = read_file(argv[1], &file_size);
    eocdr info;
    bool result = find_eocdr(&info, file_data, file_size);
    return 0;
}
