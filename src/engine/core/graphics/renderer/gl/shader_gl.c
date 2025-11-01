//
// shader_gl.c - Реализует обертку над OpenGL на основе абстрактного апи.
//


// Подключаем:
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../../math.h"
#include "../../../mm/mm.h"
#include "../../../darray.h"
#include "../../gl.h"
#include "../../shader.h"
#include "shader_gl.h"


// Объявление функций:
static void ShaderGL_Impl_compile(ShaderProgram *self);
static void ShaderGL_Impl_begin(ShaderProgram *self);
static void ShaderGL_Impl_end(ShaderProgram *self);
static int32_t ShaderGL_Impl_get_location(ShaderProgram *self, const char* name);
static void ShaderGL_Impl_set_uniform_bool(ShaderProgram *self, const char* name, bool value);
static void ShaderGL_Impl_set_uniform_int(ShaderProgram *self, const char* name, int value);
static void ShaderGL_Impl_set_uniform_float(ShaderProgram *self, const char* name, float value);
static void ShaderGL_Impl_set_uniform_vec2(ShaderProgram *self, const char* name, Vec2f value);
static void ShaderGL_Impl_set_uniform_vec3(ShaderProgram *self, const char* name, Vec3f value);
static void ShaderGL_Impl_set_uniform_vec4(ShaderProgram *self, const char* name, Vec4f value);
static void ShaderGL_Impl_set_uniform_mat2(ShaderProgram *self, const char* name, mat2 value);
static void ShaderGL_Impl_set_uniform_mat3(ShaderProgram *self, const char* name, mat3 value);
static void ShaderGL_Impl_set_uniform_mat4(ShaderProgram *self, const char* name, mat4 value);
static void ShaderGL_Impl_set_uniform_mat2x3(ShaderProgram *self, const char* name, mat2x3 value);
static void ShaderGL_Impl_set_uniform_mat3x2(ShaderProgram *self, const char* name, mat3x2 value);
static void ShaderGL_Impl_set_uniform_mat2x4(ShaderProgram *self, const char* name, mat2x4 value);
static void ShaderGL_Impl_set_uniform_mat4x2(ShaderProgram *self, const char* name, mat4x2 value);
static void ShaderGL_Impl_set_uniform_mat3x4(ShaderProgram *self, const char* name, mat3x4 value);
static void ShaderGL_Impl_set_uniform_mat4x3(ShaderProgram *self, const char* name, mat4x3 value);


// Регистрируем функции реализации апи для шейдера:
void ShaderGL_RegisterAPI(ShaderProgram *shader) {
    shader->compile = ShaderGL_Impl_compile;
    shader->begin = ShaderGL_Impl_begin;
    shader->end = ShaderGL_Impl_end;
    shader->get_location = ShaderGL_Impl_get_location;
    shader->set_uniform_bool = ShaderGL_Impl_set_uniform_bool;
    shader->set_uniform_int = ShaderGL_Impl_set_uniform_int;
    shader->set_uniform_float = ShaderGL_Impl_set_uniform_float;
    shader->set_uniform_vec2 = ShaderGL_Impl_set_uniform_vec2;
    shader->set_uniform_vec3 = ShaderGL_Impl_set_uniform_vec3;
    shader->set_uniform_vec4 = ShaderGL_Impl_set_uniform_vec4;
    shader->set_uniform_mat2 = ShaderGL_Impl_set_uniform_mat2;
    shader->set_uniform_mat3 = ShaderGL_Impl_set_uniform_mat3;
    shader->set_uniform_mat4 = ShaderGL_Impl_set_uniform_mat4;
    shader->set_uniform_mat2x3 = ShaderGL_Impl_set_uniform_mat2x3;
    shader->set_uniform_mat3x2 = ShaderGL_Impl_set_uniform_mat3x2;
    shader->set_uniform_mat2x4 = ShaderGL_Impl_set_uniform_mat2x4;
    shader->set_uniform_mat4x2 = ShaderGL_Impl_set_uniform_mat4x2;
    shader->set_uniform_mat3x4 = ShaderGL_Impl_set_uniform_mat3x4;
    shader->set_uniform_mat4x3 = ShaderGL_Impl_set_uniform_mat4x3;
}


// Реализация API:


static inline bool cmp_float(float a, float b) {
    float epsilon = 1e-6f;
    return fabsf(a-b) < epsilon;
}


static inline ShaderCacheUniformValue* find_cached_uniform(ShaderProgram *self, int loc, ShaderCacheUniformType type) {
    // TODO: Не обязательно, но лучше заменить на хэш-таблицу чтобы скорость была O(1) а не от O(n).
    for (size_t i = 0; i < DArray_len(self->uniform_values); i++) {
        ShaderCacheUniformValue *item = DArray_get(self->uniform_values, i);
        if (item->location == loc && item->type == type) return item;
    }
    return NULL;
}


static uint32_t compile_shader(ShaderProgram *program, const char* source, GLenum type) {
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* log = mm_alloc(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, log);
        // Тип шейдера:
        const char* type_str = (type == GL_VERTEX_SHADER)   ? "VERTEX"   :
                               (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :
                               (type == GL_GEOMETRY_SHADER) ? "GEOMETRY" : "UNKNOWN";
        // Сколько надо выделить памяти:
        int needed = snprintf(NULL, 0, "ShaderCompileError (%s):\n%s\n", type_str, log);
        program->error = mm_alloc(needed + 1);
        // Форматируем строку:
        sprintf(program->error, "ShaderCompileError (%s):\n%s\n", type_str, log);
        fprintf(stderr, "%s", program->error);
        mm_free(log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


static void ShaderGL_Impl_compile(ShaderProgram *self) {
    if (!self) return;

    uint32_t program = glCreateProgram();
    uint32_t shaders[3] = {0};

    // Компилируем шейдеры:
    if (self->vertex)   shaders[0] = compile_shader(self, self->vertex, GL_VERTEX_SHADER);
    if (self->fragment) shaders[1] = compile_shader(self, self->fragment, GL_FRAGMENT_SHADER);
    if (self->geometry) shaders[2] = compile_shader(self, self->geometry, GL_GEOMETRY_SHADER);

    // Линкуем программу:
    for (int i = 0; i < 3; ++i) {
        if (shaders[i]) glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    // Проверяем статус линковки:
    int linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        char* log = mm_alloc(logLength);
        glGetProgramInfoLog(program, logLength, NULL, log);
        // Сколько надо выделить памяти:
        int needed = snprintf(NULL, 0, "ShaderLinkingError:\n%s\n", log);
        self->error = mm_alloc(needed + 1);
        // Форматируем строку:
        sprintf(self->error, "ShaderCompileError:\n%s\n", log);
        fprintf(stderr, "%s", self->error);
        mm_free(log);
        glDeleteProgram(program);
        return;
    }

    // Удаляем отдельные шейдеры:
    for (int i = 0; i < 3; ++i) {
        if (shaders[i]) {
            glDetachShader(program, shaders[i]);
            glDeleteShader(shaders[i]);
        }
    }
    self->id = program;
}


static void ShaderGL_Impl_begin(ShaderProgram *self) {
    if (!self) return;
    glGetIntegerv(GL_CURRENT_PROGRAM, &self->_id_before_begin_);
    glUseProgram(self->id);
    self->_is_begin_ = true;
}


static void ShaderGL_Impl_end(ShaderProgram *self) {
    if (!self) return;
    glUseProgram((uint32_t)self->_id_before_begin_);
    self->_is_begin_ = false;
}


static int32_t ShaderGL_Impl_get_location(ShaderProgram *self, const char* name) {
    if (!self) return -1;

    // Ищем и возвращаем локацию в кэше:
    // TODO: Не обязательно, но лучше заменить на хэш-таблицу чтобы скорость была O(1) а не от O(n).
    for (size_t i = 0; i < DArray_len(self->uniform_locations); i++) {
        ShaderCacheUniformLocation *u = DArray_get(self->uniform_locations, i);
        if (strcmp(u->name, name) == 0) {
            return u->location;
        }
    }

    // Иначе получаем локацию и добавляем в кэш:
    int32_t location = glGetUniformLocation(self->id, name);
    if (location == -1) return -1;

    ShaderCacheUniformLocation *cache = mm_alloc(sizeof(ShaderCacheUniformLocation));
    cache->name = mm_strdup(name);
    cache->location = location;
    DArray_push(self->uniform_locations, cache);

    return location;
}


static void ShaderGL_Impl_set_uniform_bool(ShaderProgram *self, const char* name, bool value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_BOOL);

    // Если нашли:
    if (u) {
        if (u->vbool == value) return;  // Если значение не изменилось - выходим.
        u->vbool = value;  // Если значение изменилось - обновляем.
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_BOOL;
        cache->location = loc;
        cache->vbool = value;
        DArray_push(self->uniform_values, cache);
    }
    glUniform1i(loc, (int)value);
}


static void ShaderGL_Impl_set_uniform_int(ShaderProgram *self, const char* name, int value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_INT);

    // Если нашли:
    if (u) {
        if (u->vint == value) return;  // Если значение не изменилось - выходим.
        u->vint = value;  // Если значение изменилось - обновляем.
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_INT;
        cache->location = loc;
        cache->vint = value;
        DArray_push(self->uniform_values, cache);
    }
    glUniform1i(loc, value);
}


static void ShaderGL_Impl_set_uniform_float(ShaderProgram *self, const char* name, float value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_FLOAT);

    // Если нашли:
    if (u) {
        if (cmp_float(u->vfloat, value)) return;  // Если значение не изменилось - выходим.
        u->vfloat = value;  // Если значение изменилось - обновляем.
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_FLOAT;
        cache->location = loc;
        cache->vfloat = value;
        DArray_push(self->uniform_values, cache);
    }
    glUniform1f(loc, value);
}


static void ShaderGL_Impl_set_uniform_vec2(ShaderProgram *self, const char* name, Vec2f value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_VEC2);

    // Если нашли:
    if (u) {
        // Если значение не изменилось - выходим:
        if (cmp_float(u->vec2[0], value.x) && cmp_float(u->vec2[1], value.y)) return;
        // Если значение изменилось - обновляем:
        u->vec2[0] = value.x;
        u->vec2[1] = value.y;
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_VEC2;
        cache->location = loc;
        cache->vec2[0] = value.x;
        cache->vec2[1] = value.y;
        DArray_push(self->uniform_values, cache);
    }
    glUniform2fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_vec3(ShaderProgram *self, const char* name, Vec3f value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_VEC3);

    // Если нашли:
    if (u) {
        // Если значение не изменилось - выходим:
        if (cmp_float(u->vec3[0], value.x) && cmp_float(u->vec3[1], value.y) && cmp_float(u->vec3[2], value.z)) return;
        // Если значение изменилось - обновляем:
        u->vec3[0] = value.x;
        u->vec3[1] = value.y;
        u->vec3[2] = value.z;
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_VEC3;
        cache->location = loc;
        cache->vec3[0] = value.x;
        cache->vec3[1] = value.y;
        cache->vec3[2] = value.z;
        DArray_push(self->uniform_values, cache);
    }
    glUniform3fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_vec4(ShaderProgram *self, const char* name, Vec4f value) {
    if (!self || !name) return;
    int32_t loc = self->get_location(self, name);
    if (loc < 0) return; // Униформа не найдена.

    // Ищем униформу в кэше:
    ShaderCacheUniformValue *u = find_cached_uniform(self, loc, SHADERCACHE_UNIFORM_VEC4);

    // Если нашли:
    if (u) {
        // Если значение не изменилось - выходим:
        if (cmp_float(u->vec4[0], value.x) && cmp_float(u->vec4[1], value.y) &&
            cmp_float(u->vec4[2], value.z) && cmp_float(u->vec4[3], value.w)) return;
        // Если значение изменилось - обновляем:
        u->vec4[0] = value.x;
        u->vec4[1] = value.y;
        u->vec4[2] = value.z;
        u->vec4[3] = value.w;
    } else {  // Иначе добавляем новую запись:
        ShaderCacheUniformValue *cache = mm_alloc(sizeof(ShaderCacheUniformValue));
        cache->type = SHADERCACHE_UNIFORM_VEC4;
        cache->location = loc;
        cache->vec4[0] = value.x;
        cache->vec4[1] = value.y;
        cache->vec4[2] = value.z;
        cache->vec4[3] = value.w;
        DArray_push(self->uniform_values, cache);
    }
    glUniform4fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_mat2(ShaderProgram *self, const char* name, mat2 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3(ShaderProgram *self, const char* name, mat3 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4(ShaderProgram *self, const char* name, mat4 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat2x3(ShaderProgram *self, const char* name, mat2x3 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix2x3fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3x2(ShaderProgram *self, const char* name, mat3x2 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix3x2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat2x4(ShaderProgram *self, const char* name, mat2x4 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix2x4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4x2(ShaderProgram *self, const char* name, mat4x2 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix4x2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3x4(ShaderProgram *self, const char* name, mat3x4 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix3x4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4x3(ShaderProgram *self, const char* name, mat4x3 value) {
    if (!self || !name) return;  // Кэширование матриц и массивов слишком дорого и сложно, по этому тут нет.
    int32_t loc = self->get_location(self, name);
    glUniformMatrix4x3fv(loc, 1, GL_FALSE, (float*)value);
}
