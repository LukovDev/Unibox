//
// buffer_gc_gl.c - Создаёт код для отслеживания буферов OpenGL, которые должны быть уничтожены разом.
//


// Подключаем:
#include <stdint.h>
#include <stddef.h>
#include "../../../darray.h"
#include "../../../mm/mm.h"
#include "../../gl.h"
#include "buffer_gc_gl.h"


// Создаём единую глобальную структуру:
BufferGC_GL buffer_gc_gl = {0};


// Найти максимальный размер стеков:
static inline size_t find_max_stacks_len() {
    size_t max_stack_len = 0;
    if (DArray_len(buffer_gc_gl.qbo)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.qbo);
    if (DArray_len(buffer_gc_gl.ssbo) > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.ssbo);
    if (DArray_len(buffer_gc_gl.fbo)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.fbo);
    if (DArray_len(buffer_gc_gl.vbo)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.vbo);
    if (DArray_len(buffer_gc_gl.ibo)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.ibo);
    if (DArray_len(buffer_gc_gl.vao)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.vao);
    if (DArray_len(buffer_gc_gl.tbo)  > max_stack_len) max_stack_len = DArray_len(buffer_gc_gl.tbo);
    // ...
    return max_stack_len;
}


// Скопировать айдишки из стека в массив:
static inline void copy_ids_from_stack(DArray *stack, uint32_t *ids) {
    for (size_t i=0; i < DArray_len(stack); i++) {
        ids[i] = (uint32_t)(uintptr_t)stack->data[i];
    }
}


// Очистить стек:
static inline void stack_flush(DArray *stack) {
    DArray_clear(stack);
    DArray_shrink(stack);
}


// Инициализация стеков буферов:
void BufferGC_GL_init() {
    size_t start_capacity = 1024;  // Начальный и стандартный размер стеков.
    buffer_gc_gl.qbo  = DArray_create(start_capacity);
    buffer_gc_gl.ssbo = DArray_create(start_capacity);
    buffer_gc_gl.fbo  = DArray_create(start_capacity);
    buffer_gc_gl.vbo  = DArray_create(start_capacity);
    buffer_gc_gl.ibo  = DArray_create(start_capacity);
    buffer_gc_gl.vao  = DArray_create(start_capacity);
    buffer_gc_gl.tbo  = DArray_create(start_capacity);
    // ...
}


// Уничтожение стеков буферов:
void BufferGC_GL_destroy() {
    DArray_destroy(&buffer_gc_gl.qbo);
    DArray_destroy(&buffer_gc_gl.ssbo);
    DArray_destroy(&buffer_gc_gl.fbo);
    DArray_destroy(&buffer_gc_gl.vbo);
    DArray_destroy(&buffer_gc_gl.ibo);
    DArray_destroy(&buffer_gc_gl.vao);
    DArray_destroy(&buffer_gc_gl.tbo);
    // ...
}


// Добавить буфер на уничтожение:
void BufferGC_GL_push(BufferGC_GL_Type type, unsigned int id) {
    switch (type) {
        case BGC_GL_QBO:  DArray_push(buffer_gc_gl.qbo,  (void*)(uintptr_t)id); break;
        case BGC_GL_SSBO: DArray_push(buffer_gc_gl.ssbo, (void*)(uintptr_t)id); break;
        case BGC_GL_FBO:  DArray_push(buffer_gc_gl.fbo,  (void*)(uintptr_t)id); break;
        case BGC_GL_VBO:  DArray_push(buffer_gc_gl.vbo,  (void*)(uintptr_t)id); break;
        case BGC_GL_IBO:  DArray_push(buffer_gc_gl.ibo,  (void*)(uintptr_t)id); break;
        case BGC_GL_VAO:  DArray_push(buffer_gc_gl.vao,  (void*)(uintptr_t)id); break;
        case BGC_GL_TBO:  DArray_push(buffer_gc_gl.tbo,  (void*)(uintptr_t)id); break;
        // ...
    }
}


// Очистка всех буферов:
void BufferGC_GL_flush() {
    // Находим максимальный размер стека буферов:
    size_t max_stack_len = find_max_stacks_len();

    // Если нет буферов -> выходим (лишний раз не выделяем память):
    if (max_stack_len == 0) return;

    // Выделяем память для айдишек:
    uint32_t *ids = mm_alloc(sizeof(uint32_t) * max_stack_len);

    // Очищаем стек буферов QBO:
    if (DArray_len(buffer_gc_gl.qbo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.qbo, ids);
        glDeleteQueries(DArray_len(buffer_gc_gl.qbo), ids);
        stack_flush(buffer_gc_gl.qbo);
    }
    // Очищаем стек буферов SSBO:
    if (DArray_len(buffer_gc_gl.ssbo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.ssbo, ids);
        glDeleteBuffers(DArray_len(buffer_gc_gl.ssbo), ids);
        stack_flush(buffer_gc_gl.ssbo);
    }
    // Очищаем стек буферов FBO:
    if (DArray_len(buffer_gc_gl.fbo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.fbo, ids);
        glDeleteFramebuffers(DArray_len(buffer_gc_gl.fbo), ids);
        stack_flush(buffer_gc_gl.fbo);
    }
    // Очищаем стек буферов VBO:
    if (DArray_len(buffer_gc_gl.vbo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.vbo, ids);
        glDeleteBuffers(DArray_len(buffer_gc_gl.vbo), ids);
        stack_flush(buffer_gc_gl.vbo);
    }
    // Очищаем стек буферов IBO:
    if (DArray_len(buffer_gc_gl.ibo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.ibo, ids);
        glDeleteBuffers(DArray_len(buffer_gc_gl.ibo), ids);
        stack_flush(buffer_gc_gl.ibo);
    }
    // Очищаем стек буферов VAO:
    if (DArray_len(buffer_gc_gl.vao) > 0) {
        copy_ids_from_stack(buffer_gc_gl.vao, ids);
        glDeleteVertexArrays(DArray_len(buffer_gc_gl.vao), ids);
        stack_flush(buffer_gc_gl.vao);
    }
    // Очищаем стек буферов TBO:
    if (DArray_len(buffer_gc_gl.tbo) > 0) {
        copy_ids_from_stack(buffer_gc_gl.tbo, ids);
        glDeleteTextures(DArray_len(buffer_gc_gl.tbo), ids);
        stack_flush(buffer_gc_gl.tbo);
    }

    // ...

    // Освобожаем память:
    mm_free(ids);
}
