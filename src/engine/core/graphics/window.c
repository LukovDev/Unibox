//
// window.c - Определяет базовый апи для создания окна.
//


// Подключаем:
#include <stddef.h>
#include <stdbool.h>
#include "../mm/mm.h"
#include "../input.h"
#include "image.h"
#include "renderer.h"
#include "window.h"


// Создать конфигурацию окна. Аргументы могут быть NULL:
WinConfig* Window_create_config(
    void (*start)   (Window *self),
    void (*update)  (Window *self, Input *input, float dtime),
    void (*render)  (Window *self, Renderer *render, float dtime),
    void (*resize)  (Window *self, int width, int height),
    void (*show)    (Window *self),
    void (*hide)    (Window *self),
    void (*destroy) (Window *self)
) {
    WinConfig* config = (WinConfig*)mm_calloc(1, sizeof(WinConfig));
    if (!config) mm_alloc_error();

    // Заполняем поля (значениями по умолчанию):
    config->title = "Untitled";
    config->icon = NULL;
    config->width = 960;
    config->height = 540;
    config->x = -1;  // Выравнивание по центру экрана.
    config->y = -1;  // Выравнивание по центру экрана.
    config->vsync = false;
    config->fps = 60;
    config->visible = true;
    config->titlebar = true;
    config->resizable = true;
    config->fullscreen = false;
    config->always_top = false;
    config->min_width = 0;
    config->min_height = 0;
    config->max_width = 0;
    config->max_height = 0;

    // Заполняем функции:
    config->start = start;
    config->update = update;
    config->render = render;
    config->resize = resize;
    config->show = show;
    config->hide = hide;
    config->destroy = destroy;

    return config;
}


// Уничтожить конфигурацию окна:
void Window_destroy_config(WinConfig **config) {
    if (!config || !*config) return;
    if ((*config)->icon != NULL) Image_destroy(&(*config)->icon);
    mm_free((*config));
    *config = NULL;
}
