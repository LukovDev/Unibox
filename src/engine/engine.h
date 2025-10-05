//
// engine.h - Основной заголовочный файл движка. Включает в себя основные компоненты ядра.
//

#pragma once


// Константы и глобальные переменные:
const char* ENGINE_VERSION = "a0.0.2";


// Сокращения unsigned:
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


// Базовые включения компонентов движка:
#include "core/files.h"
#include "core/input.h"
#include "core/mm/mm.h"
#include "core/graphics/image.h"
#include "core/graphics/camera.h"

#include "core/graphics/window.h"
#include "core/graphics/window/w_sdl3.h"

#include "core/graphics/renderer.h"
#include "core/graphics/renderer/r_gl.h"
