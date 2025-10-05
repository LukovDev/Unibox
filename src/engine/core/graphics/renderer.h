//
// renderer.h - Определяет базовый апи для создания рендереров.
//

#pragma once


// Подключаем:
#include <stdint.h>


// Виды рендереров:
typedef enum RenderType {
    RENDERER_OPENGL,
} RenderType;


// Объявление структур:
typedef struct Renderer Renderer;


// Типовая структура рендерера:
typedef struct Renderer {
    // Поля:
    const char *name;
    RenderType type;
    void       *camera;  // Текущая активная камера.
    void       *data;    // Указатель на структуру данных рендерера.

    // -------------------------------- API рендерера: --------------------------------

    void (*init)  (Renderer *self);  // Инициализация рендерера (вызывается автоматически).
    void (*clear) (Renderer *self, float r, float g, float b, float a);  // Очистка окна.
    void (*camera2d_update) (Renderer *self);  // Обновляем данные матриц в шейдере по умолчанию для 2D камеры.
    void (*camera3d_update) (Renderer *self);  // Обновляем данные матриц в шейдере по умолчанию для 3D камеры.
    void (*viewport_resize) (Renderer *self, int width, int height);  // Масштабируем область просмотра.
} Renderer;
