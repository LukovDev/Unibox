//
// shader.h - Заголовочный файл для работы с шейдерами.
//

#pragma once


// Подключаем:
#include <stdint.h>
#include <stdbool.h>
#include "../math.h"


// Виды значений юниформов для кэша:
typedef enum ShaderCacheUniformType {
    SHADERCACHE_UNIFORM_BOOL,
    SHADERCACHE_UNIFORM_INT,
    SHADERCACHE_UNIFORM_FLOAT,
    SHADERCACHE_UNIFORM_VEC2,
    SHADERCACHE_UNIFORM_VEC3,
    SHADERCACHE_UNIFORM_VEC4,
    // Что-то ещё? ...
} ShaderCacheUniformType;


// Объявление структур:
typedef struct ShaderProgram ShaderProgram;
typedef struct Renderer Renderer;
typedef struct DArray DArray;
typedef struct ShaderCacheUniformLocation ShaderCacheUniformLocation;
typedef struct ShaderCacheUniformValue ShaderCacheUniformValue;


// Единица кэша локаций юниформов:
typedef struct ShaderCacheUniformLocation {
    char *name;        // Имя юниформа.
    int32_t location;  // Позиция в шейдере.
} ShaderCacheUniformLocation;


// Единица кэша значений юниформов:
typedef struct ShaderCacheUniformValue {
    ShaderCacheUniformType type;  // Тип значения.
    int32_t location;  // Позиция в шейдере.
    union {  // Данные:
        bool vbool;
        int32_t vint;
        float vfloat;
        float vec2[2];
        float vec3[3];
        float vec4[4];
        // Что-то ещё? ...
    };
} ShaderCacheUniformValue;


// Единица кэша сэмплеров:
// TODO сделать TextureUnits.
// typedef struct ShaderCacheSampler {
//     char *name;
//     uint32_t unit;
// } ShaderCacheSampler;


// Структура шейдера:
typedef struct ShaderProgram {
    const char* vertex;
    const char* fragment;
    const char* geometry;
    char* error;
    uint32_t id;
    int32_t _id_before_begin_;
    Renderer *renderer;
    bool _is_begin_;

    // Динамические списки для кэша параметров шейдера:
    DArray *uniform_locations;  // Кэш позиций uniform.
    DArray *uniform_values;     // Кэш значений uniform (всё кроме массивов и матриц).
    // DArray *sampler_units;      // Кэш привязки текстурных юнитов к названиям униформов.

    // Функции:

    void  (*compile)   (ShaderProgram *self);  // Компиляция шейдеров в программу.
    char* (*get_error) (ShaderProgram *self);  // Получить ошибку компиляции или линковки.
    void  (*begin)     (ShaderProgram *self);  // Активация программы.
    void  (*end)       (ShaderProgram *self);  // Деактивация программы.

    int32_t (*get_location) (ShaderProgram *self, const char* name);  // Получить локацию переменной.

    void (*set_uniform_bool)  (ShaderProgram *self, const char* name, bool value);   // Установить значение bool.
    void (*set_uniform_int)   (ShaderProgram *self, const char* name, int value);    // Установить значение int.
    void (*set_uniform_float) (ShaderProgram *self, const char* name, float value);  // Установить значение float.

    void (*set_uniform_vec2) (ShaderProgram *self, const char* name, Vec2f value);  // Установить значение vec2.
    void (*set_uniform_vec3) (ShaderProgram *self, const char* name, Vec3f value);  // Установить значение vec3.
    void (*set_uniform_vec4) (ShaderProgram *self, const char* name, Vec4f value);  // Установить значение vec4.

    void (*set_uniform_mat2)   (ShaderProgram *self, const char* name, mat2   value);  // Установить значение mat2.
    void (*set_uniform_mat3)   (ShaderProgram *self, const char* name, mat3   value);  // Установить значение mat3.
    void (*set_uniform_mat4)   (ShaderProgram *self, const char* name, mat4   value);  // Установить значение mat4.
    void (*set_uniform_mat2x3) (ShaderProgram *self, const char* name, mat2x3 value);  // Установить значение mat2x3.
    void (*set_uniform_mat3x2) (ShaderProgram *self, const char* name, mat3x2 value);  // Установить значение mat3x2.
    void (*set_uniform_mat2x4) (ShaderProgram *self, const char* name, mat2x4 value);  // Установить значение mat2x4.
    void (*set_uniform_mat4x2) (ShaderProgram *self, const char* name, mat4x2 value);  // Установить значение mat4x2.
    void (*set_uniform_mat3x4) (ShaderProgram *self, const char* name, mat3x4 value);  // Установить значение mat3x4.
    void (*set_uniform_mat4x3) (ShaderProgram *self, const char* name, mat4x3 value);  // Установить значение mat4x3.

    // TODO: сделать код для работы с текстурными юнитами чтобы можно было сделать функции ниже:
    // void (*set_sampler2d) (ShaderProgram *self, const char* name, uint32_t texture_id);
    // void (*set_sampler3d) (ShaderProgram *self, const char* name, uint32_t texture_id);
} ShaderProgram;


// Создать шейдерную программу:
ShaderProgram* ShaderProgram_create(Renderer *renderer, const char *vert, const char *frag, const char *geom);

// Уничтожить шейдерную программу:
void ShaderProgram_destroy(ShaderProgram **shader);
