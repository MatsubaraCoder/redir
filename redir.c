#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"


typedef enum
{
    FILE_SUCCESS,
    FILE_NULL_PARAMETER,
    FILE_OPEN_FILE_FAILED,
    FILE_CREATE_FILE_FAILED,
    FILE_MALLOC_FAILED,
    FILE_GET_SIZEOF_FILE_FAILED,
    FILE_READ_WHOLE_FILE_FAILED,
    FILE_WRITE_WHOLE_CONTENT_FAILED,
} file_error_code_t;

typedef enum
{
    DIR_NULL_PARAMETER,
    DIR_READ_DIRECTORY_FAILED,
} get_dir_error_t;

typedef struct
{
    ino_t       key;
    char        *value;
} InodeMap;


char *readfile(const char *file_path, file_error_code_t *error_code)
{
    if (!file_path)
    {
        *error_code = FILE_NULL_PARAMETER;
        return NULL;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        *error_code = FILE_OPEN_FILE_FAILED;
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long sizeof_file = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if (sizeof_file < 0)
    {
        *error_code = FILE_GET_SIZEOF_FILE_FAILED;
        fclose(file);
        return NULL;
    }

    char *file_content = malloc(sizeof_file + 1);
    if (!file_content)
    {
        *error_code = FILE_MALLOC_FAILED;
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(file_content, 1, (size_t)sizeof_file, file);
    file_content[bytes_read] = '\0';

    if ((size_t)sizeof_file != bytes_read)
    {
        *error_code = FILE_READ_WHOLE_FILE_FAILED;
        fclose(file);
        return NULL;
    }

    fclose(file);

    *error_code = FILE_SUCCESS;
    return file_content;
}

void writefile(const char *filepath, const char *content, file_error_code_t *error_code)
{
    if (!filepath || !content)
    {
        *error_code = FILE_NULL_PARAMETER;
        return;
    }

    FILE *file = fopen(filepath, "wb");
    if (!file)
    {
        *error_code = FILE_OPEN_FILE_FAILED;
        return;
    }

    size_t sizeof_content = strlen(content);
    size_t bytes_write = fwrite(content, 1, sizeof_content, file);
    fclose(file);

    if (bytes_write < sizeof_content)
    {
        *error_code = FILE_WRITE_WHOLE_CONTENT_FAILED;
        return;
    }

    *error_code = FILE_SUCCESS;
}

char *create_tempfile(file_error_code_t *error_code)
{
    char *tempfile_path = malloc(strlen("/tmp/vdir-XXXXXX") + 1);
    if (!tempfile_path)
    {
        *error_code = FILE_MALLOC_FAILED;
        return NULL;
    }

    int temp_fd = mkstemp(tempfile_path);
    if (temp_fd == -1)
    {
        *error_code = FILE_CREATE_FILE_FAILED;
        return NULL;
    }

    close(temp_fd);

    *error_code = FILE_SUCCESS;
    return tempfile_path;
}

InodeMap *get_all_directories(const char *path, get_dir_error_t *error_code)
{
    if (!path || !error_code)
    {
        *error_code = DIR_NULL_PARAMETER;
        return NULL;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        *error_code = DIR_READ_DIRECTORY_FAILED;
        return NULL;
    }

    InodeMap *directory_map = NULL;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        hmput(directory_map, entry->d_ino, strdup(entry->d_name));
    }

    closedir(dir);

    return directory_map;
}

void free_inode_map(InodeMap **map)
{
    for (ptrdiff_t i = 0; i < hmlen(*map); i++)
        free((*map)[i].value);

    hmfree(*map);
    *map = NULL;
}

int main()
{
    return 0;
}
