//
// mm.c - Исходник реализовывающий базовую работу менеджера памяти.
//
// Пока что просто обертка над обычным malloc, но позволяет отслеживать
// использование памяти, и получать размер блока памяти. Отслеживание
// памяти является атомарным, что подходит для многопоточности.
//


// Подключаем:
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include "mm.h"


// Определения функций аллокатора которые используются в этой обертке (пока что используется базовый аллокатор):
void* (*_m_alloc)   (size_t s)           = malloc;
void* (*_m_calloc)  (size_t c, size_t s) = calloc;
void* (*_m_realloc) (void *p, size_t s)  = realloc;
void  (*_m_free)    (void *p)            = free;


// Сколько памяти используется в байтах:
static const size_t _header_size = sizeof(size_t) * 8;  // Выравнивание по 8 байт для SSE, AVX/2, кэша и чётных адресов.
static atomic_size_t mm_total_allocated_blocks = 0;     // Количество выделенных блоков.
static atomic_size_t mm_used_size = 0;                  // Количество используемой виртуальной памяти.


// Получить размер заголовка блока в байтах:
size_t mm_get_block_header_size() { return _header_size; }


// Получить количество выделенных блоков:
size_t mm_get_total_allocated_blocks() { return mm_total_allocated_blocks; }


// Получить абсолютный размер используемой памяти в байтах с учётом заголовков блоков:
size_t mm_get_absolute_used_size() { return mm_used_size + _header_size * mm_total_allocated_blocks; }


// Получить сколько всего используется памяти в байтах этим менеджером памяти:
size_t mm_get_used_size() { return mm_used_size; }


// Получить сколько всего используется памяти в килобайтах этим менеджером памяти:
double mm_get_used_size_kb() { return mm_get_used_size() / 1024.0; }  // b -> kb.


// Получить сколько всего используется памяти в мегабайтах этим менеджером памяти:
double mm_get_used_size_mb() { return mm_get_used_size() / 1024.0 / 1024.0; }  // b -> kb -> mb.


// Получить сколько всего используется памяти в гигабайтах этим менеджером памяти:
double mm_get_used_size_gb() { return mm_get_used_size() / 1024.0 / 1024.0 / 1024.0; }  // b -> kb -> mb -> gb.


// Получить размер блока в байтах:
size_t mm_get_block_size(void *ptr) {
    if (!ptr) return 0;
    return *(size_t*)((char*)ptr - _header_size);
}


// Добавить байты к использованной памяти (атомарно):
void mm_used_size_add(size_t size) {
    atomic_fetch_add(&mm_used_size, size);
}


// Вычесть байты из использованной памяти (атомарно):
void mm_used_size_sub(size_t size) {
    atomic_fetch_sub(&mm_used_size, size);
}


// Выделение памяти:
void* mm_alloc(size_t size) {
    // Выделяем с запасом под данные размера блока с учетом выравнивания:
    // [размер блока в size_t|сам блок] <- весь блок.
    // ptr = (void*)(raw_ptr + _header_size) -> получить сам блок.
    // raw_ptr = (void*)(ptr - _header_size) -> получить весь блок.
    char *raw_ptr = _m_alloc(_header_size + size);
    if (!raw_ptr) { mm_alloc_error(); return NULL; }
    *(size_t*)raw_ptr = size;  // Сохраняем размер.
    void *ptr = raw_ptr + _header_size;
    mm_used_size_add(mm_get_block_size(ptr));
    mm_total_allocated_blocks++;
    return ptr;
}


// Выделение памяти с обнулением:
void* mm_calloc(size_t count, size_t size) {
    char *raw_ptr = _m_calloc(1, _header_size + count * size);
    if (!raw_ptr){ mm_alloc_error(); return NULL; }
    *(size_t*)raw_ptr = count * size;  // Сохраняем размер.
    void *ptr = raw_ptr + _header_size;
    mm_used_size_add(mm_get_block_size(ptr));
    mm_total_allocated_blocks++;
    return ptr;
}


// Расширение блока памяти:
void* mm_realloc(void *ptr, size_t new_size) {
    if (!ptr) return mm_alloc(new_size);  // Если NULL -> обычный alloc.
    void *raw_ptr = (char*)ptr - _header_size;
    void *new_raw_ptr = _m_realloc(raw_ptr, _header_size + new_size);
    if (!new_raw_ptr) { mm_alloc_error(); return NULL; }
    mm_used_size_add(new_size - *(size_t*)new_raw_ptr);
    *(size_t*)new_raw_ptr = new_size;
    return (char*)new_raw_ptr + _header_size;
}


// Копирование строки:
char* mm_strdup(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = mm_alloc(len);
    if (!copy) { mm_alloc_error(); return NULL; }
    memcpy(copy, str, len);
    return copy;
}


// Освобождение памяти:
void mm_free(void *ptr) {
    if (!ptr) return;
    mm_used_size_sub(mm_get_block_size(ptr));
    mm_total_allocated_blocks--;
    _m_free((char*)ptr - _header_size);
}


// Вызовите если получите проблему при выделении памяти:
void mm_alloc_error() {
    printf("\n\n----------------\n\n");
    printf("Memory Allocation Error!\n");
    printf("Memory used: %g kb (%zu b).\n", mm_get_used_size_kb(), mm_get_used_size());
    printf("\n----------------\n\n");
    exit(ENOMEM);
}
