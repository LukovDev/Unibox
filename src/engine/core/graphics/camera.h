//
// camera.h - Заголовочный файл для камеры.
//

#pragma once


// Подключаем:
#include <stdbool.h>
#include "../math.h"


// Объявление структур:
typedef struct Camera2D Camera2D;
typedef struct Window Window;


// Структура 2D камеры:
typedef struct Camera2D {
    Window *window;   // Указатель на окно.
    Vec2d position;   // Позиция камеры.
    float angle;      // Угол наклона камеры.
    float zoom;       // Масштаб камеры.
    float meter;      // Масштаб единицы измерения.
    bool _ui_begin_;  // Отрисовывается ли интерфейс.

    mat4 view;  // Матрица вида.
    mat4 proj;  // Матрица проекции.

    union {
        int size[2];  // Размер камеры.
        struct {
            int width;   // Ширина камеры.
            int height;  // Высота камеры.
        };
    };

    // Функции:

    void (*update)   (Camera2D *self);  // Обновление камеры.
    void (*resize)   (Camera2D *self, int width, int height);  // Изменение размера камеры.
    void (*ui_begin) (Camera2D *self);  // Начало отрисовки UI.
    void (*ui_end)   (Camera2D *self);  // Конец отрисовки UI.
} Camera2D;


// Создать 2D камеру:
Camera2D* Camera2D_create(Window *window, int width, int height, Vec2d position, float angle, float zoom);

// Уничтожить 2D камеру:
void Camera2D_destroy(Camera2D **camera);
