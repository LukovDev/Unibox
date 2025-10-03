//
// input.h - Заголовочный файл, определяющий способы получения ввода.
//

#pragma once


// Подключаем:
#include <stdbool.h>
#include <stdint.h>


// Независимые сканкоды:
typedef enum Input_Scancode {
    K_UNKNOWN,        // unknown scancode
    K_0,              // 0
    K_1,              // 1
    K_2,              // 2
    K_3,              // 3
    K_4,              // 4
    K_5,              // 5
    K_6,              // 6
    K_7,              // 7
    K_8,              // 8
    K_9,              // 9
    K_a,              // A
    K_b,              // B
    K_c,              // C
    K_d,              // D
    K_e,              // E
    K_f,              // F
    K_g,              // G
    K_h,              // H
    K_i,              // I
    K_j,              // J
    K_k,              // K
    K_l,              // L
    K_m,              // M
    K_n,              // N
    K_o,              // O
    K_p,              // P
    K_q,              // Q
    K_r,              // R
    K_s,              // S
    K_t,              // T
    K_u,              // U
    K_v,              // V
    K_w,              // W
    K_x,              // X
    K_y,              // Y
    K_z,              // Z

    K_KP_0,           // Num 0
    K_KP_1,           // Num 1
    K_KP_2,           // Num 2
    K_KP_3,           // Num 3
    K_KP_4,           // Num 4
    K_KP_5,           // Num 5
    K_KP_6,           // Num 6
    K_KP_7,           // Num 7
    K_KP_8,           // Num 8
    K_KP_9,           // Num 9
    K_KP_EXCLAIM,     // !
    K_KP_HASH,        // #
    K_KP_AMPERSAND,   // &
    K_KP_LEFTPAREN,   // (
    K_KP_RIGHTPAREN,  // )
    K_KP_PERIOD,      // Num .
    K_KP_DIVIDE,      // Num /
    K_KP_MULTIPLY,    // Num *
    K_KP_MINUS,       // Num -
    K_KP_PLUS,        // Num +
    K_KP_ENTER,       // Num Enter
    K_KP_EQUALS,      // Num =
    K_KP_COLON,       // :
    K_KP_LESS,        // <
    K_KP_GREATER,     // >
    K_KP_AT,          // @

    K_F1,             // F1
    K_F2,             // F2
    K_F3,             // F3
    K_F4,             // F4
    K_F5,             // F5
    K_F6,             // F6
    K_F7,             // F7
    K_F8,             // F8
    K_F9,             // F9
    K_F10,            // F10
    K_F11,            // F11
    K_F12,            // F12
    K_F13,            // F13
    K_F14,            // F14
    K_F15,            // F15

    K_BACKSPACE,      // \b
    K_TAB,            // \t
    K_CLEAR,          // clear
    K_RETURN,         // \r
    K_PAUSE,          // pause
    K_ESCAPE,         // ^[
    K_SPACE,          // space
    K_COMMA,          // ,
    K_MINUS,          // -
    K_PERIOD,         // .
    K_SLASH,          // /
    K_SEMICOLON,      // ;
    K_EQUALS,         // =
    K_LEFTBRACKET,    // [
    K_BACKSLASH,      // \.
    K_RIGHTBRACKET,   // ]
    K_DELETE,         // delete
    K_UP,             // ↑
    K_DOWN,           // ↓
    K_RIGHT,          // →
    K_LEFT,           // ←
    K_INSERT,         // insert
    K_HOME,           // home
    K_END,            // end
    K_PAGEUP,         // page up
    K_PAGEDOWN,       // page down
    K_NUMLOCK,        // numlock
    K_CAPSLOCK,       // capslock
    K_SCROLLLOCK,     // scroll lock
    K_RSHIFT,         // right shift
    K_LSHIFT,         // left shift
    K_RCTRL,          // right ctrl
    K_LCTRL,          // left ctrl
    K_RALT,           // right alt
    K_LALT,           // left alt
    K_RGUI,           // right Win/Cmd/Meta
    K_LGUI,           // left Win/Cmd/Meta
    K_MODE,           // mode shift
    K_HELP,           // help
    K_PRINTSCREEN,    // PrtSc
    K_SYSREQ,         // sysrq
    K_MENU,           // menu
    K_POWER,          // power

    INPUT_SCANCODE_RESERVED = 400,  // 400-500 reserved for dynamic keycodes.
    INPUT_SCANCODE_COUNT = 512,     // Not a key, just a count of the number of scancodes.
} Input_Scancode;


// Объявление структур:
typedef struct Input_Vec2i Input_Vec2i;
typedef struct Input_MouseState Input_MouseState;
typedef struct Input_KeyboardState Input_KeyboardState;
typedef struct Input Input;
typedef struct Window Window;


// Вектор двумерный целочисленный:
typedef struct Input_Vec2i {
    int x, y;
} Input_Vec2i;


// Состояние мыши:
typedef struct Input_MouseState {
    int max_keys;        // Количество кнопок (обычно равно 8 на каждый из полей состояний).
    bool *pressed;       // Нажатые кнопки.
    bool *down;          // Нажатия в этом кадре.
    bool *up;            // Отпускания в этом кадре.
    bool focused;        // Находится ли курсор над окном.
    bool visible;        // Видимость курсора.
    Input_Vec2i pos;     // Позиция курсора.
    Input_Vec2i rel;     // Смещение за кадр.
    Input_Vec2i scroll;  // Прокрутка колёсика мыши.
} Input_MouseState;


// Состояние клавиатуры:
typedef struct Input_KeyboardState {
    int max_keys;   // Количество клавиш (обычно равно 512 на каждый из полей состояний).
    bool *pressed;  // Нажатые клавиши.
    bool *down;     // Нажатия в этом кадре.
    bool *up;       // Отпускания в этом кадре.
} Input_KeyboardState;


// Типовая структура ввода:
typedef struct Input {
    Input_MouseState    *mouse;     // Состояние мыши (параметры мыши и кнопок).
    Input_KeyboardState *keyboard;  // Состояние клавиатуры (параметры клавиш).

    // -------------------------------- API ввода: --------------------------------

    // Мышь:
    bool*       (*get_mouse_pressed) (Window *self);                // Получить нажатие кнопок мыши.
    bool*       (*get_mouse_down)    (Window *self);                // Получить нажатие кнопки мыши.
    bool*       (*get_mouse_up)      (Window *self);                // Получить отжатие кнопки мыши.
    Input_Vec2i (*get_mouse_rel)     (Window *self);                // Получить смещение мыши за кадр.
    bool        (*get_mouse_focused) (Window *self);                // Получить нахождение мыши над окном.
    Input_Vec2i (*get_mouse_scroll)  (Window *self);                // Получить вращение колёсика мыши.
    void        (*set_mouse_pos)     (Window *self, int x, int y);  // Установить позицию мыши.
    Input_Vec2i (*get_mouse_pos)     (Window *self);                // Получить позицию мыши.
    void        (*set_mouse_visible) (Window *self, bool visible);  // Установить видимость мыши.
    bool        (*get_mouse_visible) (Window *self);                // Получить видимость мыши.

    // Клавиатура:
    bool* (*get_key_pressed) (Window *self);  // Получить нажатие клавиш клавиатуры.
    bool* (*get_key_down)    (Window *self);  // Получить нажатие клавиши клавиатуры.
    bool* (*get_key_up)      (Window *self);  // Получить отжатие клавиши клавиатуры.
} Input;


// Создать структуру мыши:
Input_MouseState* Input_MouseState_create(int max_keys);

// Уничтожить структуру мыши:
void Input_MouseState_destroy(Input_MouseState **mouse_state);

// Создать структуру клавиатуры:
Input_KeyboardState* Input_KeyboardState_create(int max_keys);

// Уничтожить структуру клавиатуры:
void Input_KeyboardState_destroy(Input_KeyboardState **keyboard_state);

// Создать структуру ввода:
Input* Input_create(
    Input_MouseState *mouse,
    Input_KeyboardState *keyboard,
    void (*set_mouse_pos) (Window *self, int x, int y),
    void (*set_mouse_visible) (Window *self, bool visible)
);

// Уничтожить структуру ввода:
void Input_destroy(Input **input);
