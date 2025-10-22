//
// shader.c - Файл с определением функций работы с шейдерами.
//


// Подключаем:
#include <stdio.h>
#include <string.h>
#include "../mm/mm.h"
#include "../darray.h"
#include "renderer.h"
#include "renderer/r_gl.h"
#include "shader.h"


// Объявление функций:
static char* ShaderProgram_Impl_get_error(ShaderProgram *self);


// Создать шейдерную программу:
ShaderProgram* ShaderProgram_create(Renderer *renderer, const char *vert, const char *frag, const char *geom) {
    ShaderProgram *shader = mm_calloc(1, sizeof(ShaderProgram));
    if (!shader) mm_alloc_error();

    // Заполняем поля:
    shader->renderer = renderer;
    shader->vertex = vert;
    shader->fragment = frag;
    shader->geometry = geom;
    shader->error = NULL;
    shader->id = 0;
    shader->_id_before_begin_ = 0;
    shader->get_error = ShaderProgram_Impl_get_error;
    shader->uniform_locations = DArray_create(92);
    shader->uniform_values = DArray_create(92);
    // shader->sampler_units = DArray_create(92);

    // Регистрируем функции для определенного рендерера:
    switch (renderer->type) {
        case RENDERER_OPENGL:
            RendererGL_RegisterShaderProgramAPI(shader);
            break;

        // Other renderers.

        default: {
            const char* err = "Unknown renderer type.";
            fprintf(stderr, "ShaderProgram_create: %s\n", err);
            shader->error = mm_alloc(strlen(err) + 1);
            if (!shader->error) mm_alloc_error();
            memcpy(shader->error, err, strlen(err) + 1);
            return shader;
        }
    }
    return shader;
}


// Уничтожить шейдерную программу:
void ShaderProgram_destroy(ShaderProgram **shader) {
    if (!shader || !*shader) return;

    // Освобождаем кэш:
    if ((*shader)->uniform_locations) {
        for (size_t i = 0; i < DArray_len((*shader)->uniform_locations); i++) {
            ShaderCacheUniformLocation *u = DArray_get((*shader)->uniform_locations, i);
            if (u) {
                mm_free(u->name);
                mm_free(u);
            }
        }
        DArray_destroy(&(*shader)->uniform_locations);
    }
    if ((*shader)->uniform_values) {
        for (size_t i = 0; i < DArray_len((*shader)->uniform_values); i++) {
            ShaderCacheUniformValue *u = DArray_get((*shader)->uniform_values, i);
            if (u) mm_free(u);
        }
        DArray_destroy(&(*shader)->uniform_values);
    }
    // if ((*shader)->sampler_units) {
    //     // ...
    //     DArray_destroy(&(*shader)->sampler_units);
    // }

    if ((*shader)->error) mm_free((*shader)->error);
    mm_free(*shader);
    *shader = NULL;
}


// Реализация API:


static char* ShaderProgram_Impl_get_error(ShaderProgram *self) {
    if (!self) return NULL;
    return self->error;
}
