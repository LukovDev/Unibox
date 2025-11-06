//
// buffer_gc_gl.h
//

#pragma once


// Подключаем:
#include "../../../darray.h"


// Типы буферов на уничтожение:
typedef enum BufferGC_GL_Type {
    BGC_GL_QBO,
    BGC_GL_SSBO,
    BGC_GL_FBO,
    BGC_GL_VBO,
    BGC_GL_IBO,
    BGC_GL_VAO,
    BGC_GL_TBO,
    // ...
} BufferGC_GL_Type;


// Объявление структур:
typedef struct BufferGC_GL BufferGC_GL;


// Единица кэша локаций юниформов:
typedef struct BufferGC_GL {
    DArray *qbo;
    DArray *ssbo;
    DArray *fbo;
    DArray *vbo;
    DArray *ibo;
    DArray *vao;
    DArray *tbo;
    // ...
} BufferGC_GL;


// Создаём единую глобальную структуру:
extern BufferGC_GL buffer_gc_gl;


// Инициализация стеков буферов:
void BufferGC_GL_init();

// Уничтожение стеков буферов:
void BufferGC_GL_destroy();

// Добавить буфер на уничтожение:
void BufferGC_GL_push(BufferGC_GL_Type type, unsigned int id);

// Очистка всех буферов:
void BufferGC_GL_flush();
