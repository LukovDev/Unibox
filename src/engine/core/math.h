//
// math.h - Заголовочный файл, который определяет включения математики и определения других типов данных.
//

#pragma once


// Подключаем:
#include "std.h"
#include <cglm/cglm.h>


// Вектор двумерный целочисленный:
typedef struct Vec2i { int x, y; } Vec2i;

// Вектор трехмерный целочисленный:
typedef struct Vec3i { int x, y, z; } Vec3i;

// Вектор четырехмерный целочисленный:
typedef struct Vec4i { int x, y, z, w; } Vec4i;


// Вектор двумерный вещественный:
typedef struct Vec2f { float x, y; } Vec2f;

// Вектор трехмерный вещественный:
typedef struct Vec3f { float x, y, z; } Vec3f;

// Вектор четырехмерный вещественный:
typedef struct Vec4f { float x, y, z, w; } Vec4f;


// Вектор двумерный вещественный с двойной точностью:
typedef struct Vec2d { double x, y; } Vec2d;

// Вектор трехмерный вещественный с двойной точностью:
typedef struct Vec3d { double x, y, z; } Vec3d;

// Вектор четырехмерный вещественный с двойной точностью:
typedef struct Vec4d { double x, y, z, w; } Vec4d;
