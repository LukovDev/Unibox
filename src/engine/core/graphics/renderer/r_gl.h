//
// r_gl.h
//

#pragma once


// Подключаем:
#include <stdbool.h>
#include "../renderer.h"


// Виды профилей:
typedef enum RendererGL_Profile {
    RENDERER_GL_CORE,
    RENDERER_GL_COMPATIBILITY,
} RendererGL_Profile;


// Объявление структур:
typedef struct RendererGL_Data RendererGL_Data;
typedef struct ShaderProgram ShaderProgram;


// Структура данных рендерера:
typedef struct RendererGL_Data {
    int minor;
    int major;
    bool doublebuffer;
    RendererGL_Profile profile;
} RendererGL_Data;


// Регистрируем функции реализации апи для шейдера:
void RendererGL_RegisterShaderProgramAPI(ShaderProgram *shader);


// Создать рендерер:
Renderer* RendererGL_create(int major, int minor, bool doublebuffer, RendererGL_Profile profile);

// Уничтожить рендерер:
void RendererGL_destroy(Renderer **self);
