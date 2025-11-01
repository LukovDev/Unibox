//
// image.h
//

#pragma once


// Подключаем:
#include <stdbool.h>


// Определяем глобальные переменные стандартной картинки:
extern const unsigned char Image_default_icon[];
extern const size_t Image_default_icon_size;
extern const int Image_default_icon_width;
extern const int Image_default_icon_height;


// Размеры каналов в байтах:
#define IMG_R    1
#define IMG_RG   2
#define IMG_RGB  3
#define IMG_RGBA 4

// Структура картинки:
typedef struct Image {
    int width;
    int height;
    int channels;
    bool from_stbi;
    unsigned char* data;
} Image;


// Загрузить картинку:
Image* Image_load(const char *filepath, int format);

// Сохранить картинку:
bool Image_save(Image* image, const char *filepath, const char *format);

// Копировать картинку в памяти:
Image* Image_copy(const Image *source);

// Создать стандартную картинку:
Image* Image_create_default();

// Освободить память картинки:
void Image_destroy(Image** image);

// Получить размер картинки в байтах:
size_t Image_get_size(Image* image);
