//
// r_gl.c - Реализует обертку над OpenGL на основе абстрактного апи.
//


// Подключаем:
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../mm/mm.h"
#include "../renderer.h"
#include "../gl.h"
#include "r_gl.h"


// Объявление функций:
static void RendererGL_Impl_init(Renderer *renderer);
static void RendererGL_Impl_clear(Renderer *renderer, float r, float g, float b, float a);


// Регистрируем функции реализации апи:
static void RendererGL_RegisterAPI(Renderer *renderer) {
    renderer->init = RendererGL_Impl_init;
    renderer->clear = RendererGL_Impl_clear;
}


// Создать рендерер:
Renderer* RendererGL_create(int major, int minor, bool doublebuffer, RendererGL_Profile profile) {
    Renderer *renderer = (Renderer*)mm_alloc(sizeof(Renderer));
    if (!renderer) mm_alloc_error();

    // Создаём данные рендерера:
    RendererGL_Data *data = (RendererGL_Data*)mm_calloc(1, sizeof(RendererGL_Data));
    if (!data) mm_alloc_error();

    // Заполняем поля данных:
    data->major = major;
    data->minor = minor;
    data->doublebuffer = doublebuffer;
    data->profile = profile;

    // Заполняем поля рендерера:
    renderer->name = "OpenGL";
    renderer->type = RENDERER_OPENGL;
    renderer->data = data;

    // Регистрируем функции:
    RendererGL_RegisterAPI(renderer);

    return renderer;
}


// Уничтожить рендерер:
void RendererGL_destroy(Renderer **renderer) {
    if (!renderer || !*renderer) return;

    // Освобождаем память данных рендерера:
    if ((*renderer)->data) {
        mm_free((*renderer)->data);
        (*renderer)->data = NULL;
    }

    // Освободить память рендерера:
    mm_free(*renderer);
    *renderer = NULL;
}


// Реализация API:


static void RendererGL_Impl_init(Renderer *renderer) {
    if (!gladLoadGL()) {
        fprintf(stderr, "R_GL-FAIL: gladLoadGL failed.\n");
        exit(1);
        return;
    }

    glEnable(GL_BLEND);  // Включаем смешивание цветов.

    // Устанавливаем режим смешивания:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Разрешаем установку размера точки через шейдер:
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Делаем нулевой текстурный юнит привязанным к нулевой текстуре:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); 
}


static void RendererGL_Impl_clear(Renderer *renderer, float r, float g, float b, float a) {
    if (!renderer) return;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
