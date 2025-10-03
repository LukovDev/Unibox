//
// files.c - Реализует работу с файлами.
//


// Подключаем:
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "mm/mm.h"
#include "files.h"


// Загружаем файл в строку:
char* fs_load_file(const char* file_path, const char* mode) {
    FILE* f = fopen(file_path, mode);
    if (!f) return NULL;

    // Определяем размер файла:
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    // Выделяем память +1 для '\0':
    char* buffer = (char*)mm_alloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    // Читаем файл:
    size_t read_size = fread(buffer, 1, size, f);
    buffer[read_size] = '\0';
    fclose(f);

    return buffer;
}


// Сохраняем строку в файл:
bool fs_save_file(const char* file_path, const char* data, const char* mode) {
    FILE* f = fopen(file_path, mode);
    if (!f) return false;

    fwrite(data, 1, strlen(data), f);
    fclose(f);
    return true;
}


// Загружаем файл в буфер бинарно:
unsigned char* fs_load_file_bin(const char* file_path, const char* mode, size_t* out_size) {
    FILE* f = fopen(file_path, mode);
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    unsigned char* buffer = (unsigned char*)mm_alloc(size);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, f);
    fclose(f);

    if (out_size) *out_size = read_size;
    return buffer;
}


// Сохраняем буфер в файл бинарно:
bool fs_save_file_bin(const char* file_path, const void* data, size_t size, const char* mode) {
    FILE* f = fopen(file_path, mode);
    if (!f) return false;

    fwrite(data, 1, size, f);
    fclose(f);
    return true;
}
