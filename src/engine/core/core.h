//
// core.h - Заголовочный файл ядра. Подключает все необходимые компоненты ядра.
//

#pragma once


// Константы и глобальные переменные:

// Имя ядра. A/B/R - Alpha, Beta, Release. 000-999+ - версия кода ядра. Остальное - название проекта.
const char* CORE_NAME = "A002-UNIBOX";


// Базовые включения компонентов движка:

// Основное:
#include "std.h"
#include "darray.h"
#include "files.h"
#include "input.h"
#include "math.h"
#include "time.h"
#include "mm/mm.h"

// Графика:
#include "graphics/camera.h"
#include "graphics/image.h"
#include "graphics/renderer.h"
#include "graphics/shader.h"
#include "graphics/window.h"

// Окна:
#include "graphics/window/w_sdl3.h"
// ...

// Рендереры:
#include "graphics/renderer/gl/renderer_gl.h"
// ...
