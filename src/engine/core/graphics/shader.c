//
// shader.c - Файл с определением функций работы с шейдерами.
//


// Подключаем:
#include <stdio.h>
#include <string.h>
#include "../mm/mm.h"
#include "renderer.h"
#include "renderer/r_gl.h"
#include "shader.h"


// Объявление функций:
static char* ShaderProgram_Impl_get_error(ShaderProgram *self);


// Создать шейдерную программу:
ShaderProgram* ShaderProgram_create(Renderer *renderer, const char* vert, const char* frag, const char* geom) {
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
            memcpy(shader->error, err, strlen(err) + 1);
            return shader;
        }
    }
    return shader;
}


// Уничтожить шейдерную программу:
void ShaderProgram_destroy(ShaderProgram **shader) {
    if (!shader || !*shader) return;
    if ((*shader)->error) mm_free((*shader)->error);
    mm_free(*shader);
    *shader = NULL;
}


// Реализация API:


static char* ShaderProgram_Impl_get_error(ShaderProgram *self) {
    if (!self) return NULL;
    return self->error;
}
