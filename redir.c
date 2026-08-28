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
    CREATE_FILE_FAILED,
    MALLOC_FAILED,
    GET_SIZEOF_FILE_FAILED,
    READ_WHOLE_FILE_FAILED,
    WRITE_WHOLE_CONTENT_FAILED,
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

void writefile(const char *filepath, const char *content, file_error_code_t *error_code)
{
    if (!filepath || !content)
    {
        *error_code = NULL_PARAMETER;
        return;
    }

    FILE *file = fopen(filepath, "wb");
    if (!file)
    {
        *error_code = OPEN_FILE_FAILED;
        return;
    }

    size_t sizeof_content = strlen(content);
    size_t bytes_write = fwrite(content, 1, sizeof_content, file);
    fclose(file);

    if (bytes_write < sizeof_content)
    {
        *error_code = WRITE_WHOLE_CONTENT_FAILED;
        return;
    }

    *error_code = SUCCESS;
}

char *create_tempfile(file_error_code_t *error_code)
{
    char *tempfile_path = malloc(strlen("/tmp/vdir-XXXXXX") + 1);
    if (!tempfile_path)
    {
        *error_code = MALLOC_FAILED;
        return NULL;
    }

    int temp_fd = mkstemp(tempfile_path);
    if (temp_fd == -1)
    {
        *error_code = CREATE_FILE_FAILED;
        return NULL;
    }

    close(temp_fd);

    *error_code = SUCCESS;
    return tempfile_path;
}


int main()
{
    return 0;
}
