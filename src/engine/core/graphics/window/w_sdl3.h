//
// w_sdl3.h
//

#pragma once


// Подключаем:
#include "../renderer.h"
#include "../window.h"


// Создать окно:
Window* WindowSDL3_create(WinConfig *config, Renderer *renderer);

// Уничтожить окно:
void WindowSDL3_destroy(Window **window);
