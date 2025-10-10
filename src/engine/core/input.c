//
// input.c - Реализовывает базовые функции для работы со структурами.
//


// Подключаем:
#include "mm/mm.h"
#include "graphics/window.h"
#include "math.h"
#include "input.h"


// Объявление функций:
static bool* Input_Impl_get_mouse_pressed(Window *self);
static bool* Input_Impl_get_mouse_down(Window *self);
static bool* Input_Impl_get_mouse_up(Window *self);
static Vec2i Input_Impl_get_mouse_rel(Window *self);
static bool Input_Impl_get_mouse_focused(Window *self);
static Vec2i Input_Impl_get_mouse_wheel(Window *self);
static Vec2i Input_Impl_get_mouse_pos(Window *self);
static bool Input_Impl_get_mouse_visible(Window *self);
static bool* Input_Impl_get_key_pressed(Window *self);
static bool* Input_Impl_get_key_down(Window *self);
static bool* Input_Impl_get_key_up(Window *self);


// Создать структуру мыши:
Input_MouseState* Input_MouseState_create(int max_keys) {
    Input_MouseState *ms = (Input_MouseState*)mm_calloc(1, sizeof(Input_MouseState));
    if (!ms) mm_alloc_error();
    ms->max_keys = max_keys;
    ms->visible = true;
    ms->pressed = (bool*)mm_calloc(max_keys, sizeof(bool));
    ms->down    = (bool*)mm_calloc(max_keys, sizeof(bool));
    ms->up      = (bool*)mm_calloc(max_keys, sizeof(bool));
    if (!ms->pressed || !ms->down || !ms->up) {
        if (ms->pressed) mm_free(ms->pressed);
        if (ms->down) mm_free(ms->down);
        if (ms->up) mm_free(ms->up);
        mm_free(ms);
        mm_alloc_error();
    }
    return ms;
}


// Уничтожить структуру мыши:
void Input_MouseState_destroy(Input_MouseState **mouse_state) {
    if (!mouse_state || !*mouse_state) return;
    if ((*mouse_state)->pressed) mm_free((*mouse_state)->pressed);
    if ((*mouse_state)->down) mm_free((*mouse_state)->down);
    if ((*mouse_state)->up) mm_free((*mouse_state)->up);
    mm_free(*mouse_state);
    *mouse_state = NULL;
}


// Создать структуру клавиатуры:
Input_KeyboardState* Input_KeyboardState_create(int max_keys) {
    Input_KeyboardState *kb = (Input_KeyboardState*)mm_calloc(1, sizeof(Input_KeyboardState));
    if (!kb) mm_alloc_error();
    kb->max_keys = max_keys;
    kb->pressed = (bool*)mm_calloc(max_keys, sizeof(bool));
    kb->down    = (bool*)mm_calloc(max_keys, sizeof(bool));
    kb->up      = (bool*)mm_calloc(max_keys, sizeof(bool));
    if (!kb->pressed || !kb->down || !kb->up) {
        if (kb->pressed) mm_free(kb->pressed);
        if (kb->down) mm_free(kb->down);
        if (kb->up) mm_free(kb->up);
        mm_free(kb);
        mm_alloc_error();
    }
    return kb;
}


// Уничтожить структуру клавиатуры:
void Input_KeyboardState_destroy(Input_KeyboardState **keyboard_state) {
    if (!keyboard_state || !*keyboard_state) return;
    if ((*keyboard_state)->pressed) mm_free((*keyboard_state)->pressed);
    if ((*keyboard_state)->down) mm_free((*keyboard_state)->down);
    if ((*keyboard_state)->up) mm_free((*keyboard_state)->up);
    mm_free(*keyboard_state);
    *keyboard_state = NULL;
}


// Создать структуру ввода:
Input* Input_create(
    Input_MouseState *mouse,
    Input_KeyboardState *keyboard,
    void (*set_mouse_pos) (Window *self, int x, int y),
    void (*set_mouse_visible) (Window *self, bool visible)
) {
    Input *input = (Input*)mm_alloc(sizeof(Input));
    if (!input) mm_alloc_error();

    // Заполняем поля:
    input->mouse = mouse;
    input->keyboard = keyboard;

    // Регистрируем функции:
    input->get_mouse_pressed = Input_Impl_get_mouse_pressed;
    input->get_mouse_down = Input_Impl_get_mouse_down;
    input->get_mouse_up = Input_Impl_get_mouse_up;
    input->get_mouse_rel = Input_Impl_get_mouse_rel;
    input->get_mouse_focused = Input_Impl_get_mouse_focused;
    input->get_mouse_wheel = Input_Impl_get_mouse_wheel;
    input->set_mouse_pos = set_mouse_pos;  // Функция зависит от реализации окна. Укажите сами.
    input->get_mouse_pos = Input_Impl_get_mouse_pos;
    input->set_mouse_visible = set_mouse_visible;  // Функция зависит от реализации окна. Укажите сами.
    input->get_mouse_visible = Input_Impl_get_mouse_visible;
    input->get_key_pressed = Input_Impl_get_key_pressed;
    input->get_key_down = Input_Impl_get_key_down;
    input->get_key_up = Input_Impl_get_key_up;

    return input;
}


// Уничтожить структуру ввода:
void Input_destroy(Input **input) {
    if (!input || !*input) return;
    // Input_KeyboardState_destroy(&(*input)->keyboard);
    // Input_MouseState_destroy(&(*input)->mouse);
    mm_free(*input);
    *input = NULL;
}


// Реализация API:


static bool* Input_Impl_get_mouse_pressed(Window *self) {
    if (!self || !self->input || !self->input->mouse) return NULL;
    return self->input->mouse->pressed;
}


static bool* Input_Impl_get_mouse_down(Window *self) {
    if (!self || !self->input || !self->input->mouse) return NULL;
    return self->input->mouse->down;
}


static bool* Input_Impl_get_mouse_up(Window *self) {
    if (!self || !self->input || !self->input->mouse) return NULL;
    return self->input->mouse->up;
}


static Vec2i Input_Impl_get_mouse_rel(Window *self) {
    if (!self || !self->input || !self->input->mouse) return (Vec2i){0, 0};
    return self->input->mouse->rel;
}


static bool Input_Impl_get_mouse_focused(Window *self) {
    if (!self || !self->input || !self->input->mouse) return false;
    return self->input->mouse->focused;
}


static Vec2i Input_Impl_get_mouse_wheel(Window *self) {
    if (!self || !self->input || !self->input->mouse) return (Vec2i){0, 0};
    return self->input->mouse->wheel;
}


static Vec2i Input_Impl_get_mouse_pos(Window *self) {
    if (!self || !self->input || !self->input->mouse) return (Vec2i){0, 0};
    return self->input->mouse->pos;
}


static bool Input_Impl_get_mouse_visible(Window *self) {
    if (!self || !self->input || !self->input->mouse) return false;
    return self->input->mouse->visible;
}


static bool* Input_Impl_get_key_pressed(Window *self) {
    if (!self || !self->input || !self->input->keyboard) return NULL;
    return self->input->keyboard->pressed;
}


static bool* Input_Impl_get_key_down(Window *self) {
    if (!self || !self->input || !self->input->keyboard) return NULL;
    return self->input->keyboard->down;
}


static bool* Input_Impl_get_key_up(Window *self) {
    if (!self || !self->input || !self->input->keyboard) return NULL;
    return self->input->keyboard->up;
}
