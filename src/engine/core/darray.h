//
// darray.h - Dynamic Array реализация в си.
//

#pragma once


// Подключаем:
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>


// Определения:
#define DARRAY_DEFAULT_CAPACITY 512  // Размер массива по умолчанию.


// Объявление структур:
typedef struct DArray DArray;


// Структура динамического массива:
typedef struct DArray {
    void **data;       // Массив указателей.
    size_t len;        // Длина массива (сколько ячеек занято).
    size_t capacity;   // Всего выделенных ячеек в памяти.
    size_t init_cap;   // Размер массива по умолчанию.
} DArray;


// Создать динамический массив с заданным размером:
DArray* DArray_create(size_t initial_capacity);

// Добавить элемент в массив:
void DArray_push(DArray *arr, void *element);

// Ужимаем массив при необходимости:
void DArray_shrink(DArray *arr);

// Получение элемента по индексу:
void* DArray_get(DArray *arr, size_t index);

// Получение элемента по индексу по кругу (индексация замкнута):
void* DArray_get_round(DArray *arr, size_t index);

// Вставка элемента по индексу со сдвигом:
void DArray_insert(DArray *arr, size_t index, void *element);

// Переворот массива:
void DArray_reverse(DArray *arr);

// Печать содержимого массива:
void DArray_print(DArray *arr, FILE *out, bool int_mode);

// Получить длину массива:
size_t DArray_len(DArray *arr);

// Удаление элемента со сдвигом:
// ВНИМАНИЕ: Удаление указателя из массива не удаляет сам блок памяти! Освобождай вручную!
void* DArray_remove(DArray *arr, size_t index);

// Получить и удалить последний элемент из массива:
// ВНИМАНИЕ: Удаление указателя из массива не удаляет сам блок памяти! Освобождай вручную!
void* DArray_pop(DArray *arr);

// Очистка массива:
// ВНИМАНИЕ: Очистка массива не удаляет сами блоки памяти указателей! Освобождай вручную!
void DArray_clear(DArray *arr);

// Уничтожение массива:
// ВНИМАНИЕ: Блоки памяти указателей не удаляются! Освобождай вручную!
void DArray_destroy(DArray **arr);
