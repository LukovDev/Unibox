//
// files.h - Заголовочный файл для работы с файлами.
//

#pragma once


// Подключаем:
#include <stdbool.h>


// Загружаем файл в строку:
char* fs_load_file(const char* file_path, const char* mode);

// Сохраняем строку в файл:
bool fs_save_file(const char* file_path, const char* data, const char* mode);

// Загружаем файл в буфер бинарно:
unsigned char* fs_load_file_bin(const char* file_path, const char* mode, size_t* out_size);

// Сохраняем буфер в файл бинарно:
bool fs_save_file_bin(const char* file_path, const void* data, size_t size, const char* mode);
