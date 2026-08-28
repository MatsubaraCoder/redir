#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


typedef enum
{
    SUCCESS,
    NULL_PARAMETER,
    OPEN_FILE_FAILED,
    CREATE_TEMPFILE_FAILED,
    MALLOC_FAILED,
    GET_SIZEOF_FILE_FAILED,
    READ_WHOLE_FILE_FAILED,
} file_error_code_t;


char *readfile(const char *file_path, file_error_code_t *error_code)
{
    if (!file_path)
    {
        *error_code = NULL_PARAMETER;
        return NULL;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        *error_code = OPEN_FILE_FAILED;
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long sizeof_file = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (sizeof_file < 0)
    {
        *error_code = GET_SIZEOF_FILE_FAILED;
        fclose(file);
        return NULL;
    }

    char *file_content = malloc(sizeof_file + 1);
    if (!file_content)
    {
        *error_code = MALLOC_FAILED;
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(file_content, 1, (size_t)sizeof_file, file);
    file_content[bytes_read] = '\0';

    if ((size_t)sizeof_file != bytes_read)
    {
        *error_code = READ_WHOLE_FILE_FAILED;
        fclose(file);
        return NULL;
    }

    fclose(file);

    *error_code = SUCCESS;
    return file_content;
}


int main()
{
    return 0;
}
