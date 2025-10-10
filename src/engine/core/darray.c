//
// darray.c - Dynamic Array реализация в си.
//


// Подключаем:
#include <stddef.h>
#include <string.h>
#include "mm/mm.h"
#include "darray.h"


// Расширяем массив автоматически:
static void DArray_grow(DArray *arr) {
    if (!arr) return;

    // Если свободной памяти в массиве нет - увеличиваем массив в 2 раза:
    if (arr->len >= arr->capacity) {
        size_t new_capacity = arr->capacity * 2;  // В 2 раза больше от текущего.
        if (new_capacity <= arr->capacity) new_capacity = arr->capacity + 1; // Защита от overflow.
        arr->capacity = new_capacity;
        arr->data = mm_realloc(arr->data, sizeof(void*) * arr->capacity);
        if (!arr->data) mm_alloc_error();
    }
}


// Создать динамический массив с заданным размером:
DArray* DArray_create(size_t initial_capacity) {
    if (initial_capacity == 0) {
        initial_capacity = DARRAY_DEFAULT_CAPACITY;
    }

    DArray *arr = (DArray*)mm_alloc(sizeof(DArray));
    if (!arr) mm_alloc_error();

    arr->data = mm_calloc(initial_capacity, sizeof(void*));
    if (!arr->data) { mm_free(arr); mm_alloc_error(); }

    arr->len = 0;
    arr->capacity = initial_capacity;
    return arr;
}


// Добавить элемент в массив:
void DArray_push(DArray *arr, void *element) {
    if (!arr) return;

    // Проверяем вместимость массива:
    DArray_grow(arr);

    // Добавляем новый элемент, потом пост-инкрементируем длину массива:
    arr->data[arr->len++] = element;
}


// Ужимаем массив при необходимости:
void DArray_shrink(DArray *arr) {
    if (!arr) return;

    // Если массив пустой, задаем дефолтный размер:
    if (arr->len == 0) {
        arr->capacity = DARRAY_DEFAULT_CAPACITY;
        arr->data = mm_realloc(arr->data, sizeof(void*) * arr->capacity);
        if (!arr->data) mm_alloc_error();
        return;
    }

    // Целевой размер массива: занятые элементы + 25% текущей capacity.
    // Это оставляет немного свободного пространства для будущих вставок,
    // чтобы уменьшить количество повторных realloc при добавлении новых элементов.
    size_t target_capacity = arr->len + arr->capacity / 4;

    // Уменьшаем только если экономия существенна (иначе realloc не имеет смысла и может ухудшить производительность):
    if (target_capacity < arr->capacity) {
        arr->capacity = target_capacity;
        arr->data = mm_realloc(arr->data, sizeof(void*) * arr->capacity);
        if (!arr->data) mm_alloc_error();
    }
}


// Получение элемента по индексу:
void* DArray_get(DArray *arr, size_t index) {
    if (!arr || index >= arr->len) return NULL;
    return arr->data[index];
}


// Получение элемента по индексу по кругу (индексация замкнута):
void* DArray_get_round(DArray *arr, size_t index) {
    if (!arr || arr->len == 0) return NULL;
    return arr->data[index % arr->len];
}


// Вставка элемента по индексу со сдвигом:
void DArray_insert(DArray *arr, size_t index, void *element) {
    if (!arr) return;
    if (index > arr->len) index = arr->len;

    // Проверяем вместимость массива:
    DArray_grow(arr);

    // Сдвигаем всё вправо от index до конца:
    memmove(&arr->data[index + 1], &arr->data[index], (arr->len - index) * sizeof(void*));

    // Вставляем элемент:
    arr->data[index] = element;
    arr->len++;
}


// Переворот массива:
void DArray_reverse(DArray *arr) {
    if (!arr || arr->len < 2) return;

    size_t i = 0;
    size_t j = arr->len - 1;
    while (i < j) {
        void *tmp = arr->data[i];
        arr->data[i] = arr->data[j];
        arr->data[j] = tmp;
        i++;
        j--;
    }
}


// Удаление элемента со сдвигом:
// ВНИМАНИЕ: Удаление указателя из массива не удаляет сам блок памяти! Освобождай вручную!
void* DArray_remove(DArray *arr, size_t index) {
    if (!arr || index >= arr->len) return NULL;

    void* ptr = arr->data[index];  // Получаем указатель перед его удалением из массива.

    // Сдвиг влево:
    memmove(&arr->data[index], &arr->data[index + 1], (arr->len - index - 1) * sizeof(void*));
    arr->len--;

    // Ужимаем массив при необходимости:
    // DArray_shrink(arr);

    return ptr;
}


// Получить и удалить последний элемент из массива:
// ВНИМАНИЕ: Удаление указателя из массива не удаляет сам блок памяти! Освобождай вручную!
void* DArray_pop(DArray *arr) {
    if (!arr || arr->len == 0) return NULL;
    return DArray_remove(arr, arr->len-1);  // Удаляем последний элемент и возвращаем его.
}


// Очистка массива:
// ВНИМАНИЕ: Очистка массива не удаляет сами блоки памяти указателей! Освобождай вручную!
void DArray_clear(DArray *arr) {
    if (!arr) return;
    arr->len = 0;
}


// Уничтожение массива:
// ВНИМАНИЕ: Блоки памяти указателей не удаляются! Освобождай вручную!
void DArray_destroy(DArray **arr) {
    if (!arr || !*arr) return;

    mm_free((*arr)->data);
    mm_free(*arr);
    *arr = NULL;
}
