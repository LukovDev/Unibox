//
// texture.h - Заголовочный файл для работы с текстурами.
//

#pragma once


// Подключаем:
#include <stdbool.h>


// Типы формата данных текстуры и исходников:
typedef enum TextureFormat {
    TEX_RED,    TEX_RG,
    TEX_RGB,    TEX_RGBA,
    TEX_RGB16F, TEX_RGBA16F,
    TEX_RGB32F, TEX_RGBA32F,
    TEX_R16F,   TEX_SRGB,
    TEX_SRGBA,  TEX_BGR,
    TEX_BGRA,
} TextureFormat;


// Типы данных используемой в текстуре:
typedef enum TextureDataType {
    TEX_DATA_UBYTE,  TEX_DATA_BYTE,
    TEX_DATA_USHORT, TEX_DATA_SHORT,
    TEX_DATA_UINT,   TEX_DATA_INT,
    TEX_DATA_FLOAT,
} TextureDataType;


// Объявление структур:
typedef struct Texture Texture;
typedef struct Renderer Renderer;
typedef struct Image Image;


// Структура текстуры:
typedef struct Texture {
    Renderer *renderer;
    uint32_t id;
    int width;
    int height;
    int channels;
    bool _is_begin_;
    int32_t _id_before_begin_;

    // Функции:

    void (*begin) (Texture *self);  // Активация текстуры.
    void (*end)   (Texture *self);  // Деактивация текстуры.
    void (*load)  (Texture *self, Image *image);  // Загрузить текстуру из картинки.

    // Установить данные текстуры:
    void (*set_data) (Texture *self, const int width, const int height, const void *data, bool use_mipmap,
                      TextureFormat tex_format, TextureFormat data_format, TextureDataType data_type);

    Image* (*get_image)   (Texture *self, int channels);  // Получить картинку из текстуры.
    void (*set_filter)    (Texture *self, int name, int param);  // Установить фильтрацию текстуры.
    void (*set_linear)    (Texture *self);  // Установить линейную фильтрацию текстуры.
    void (*set_pixelized) (Texture *self);  // Установить пикселизацию текстуры.
    void (*_destroy_)     (Texture *self);  // Внутренняя функция для удаления самой текстуры.
} Texture;

// Создать текстуру:
Texture* Texture_create(Renderer *renderer);

// Уничтожить текстуру:
void Texture_destroy(Texture **texture);
