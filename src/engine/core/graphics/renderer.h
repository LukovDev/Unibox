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
    void       *data;  // Указатель на структуру данных рендерера.

    // -------------------------------- API рендерера: --------------------------------

    void (*init)(Renderer *renderer);  // Инициализация рендерера (вызывается автоматически).
    void (*clear)(Renderer *renderer, float r, float g, float b, float a);  // Очистка окна.
} Renderer;
