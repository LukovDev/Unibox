//
// w_sdl3.c - Реализует обертку над SDL3 на основе абстрактного апи.
//


// Подключаем:
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <SDL3/SDL.h>
#include "../../mm/mm.h"
#include "../../math.h"
#include "../../input.h"
#include "../image.h"
#include "../renderer.h"
#include "../renderer/gl/renderer_gl.h"
#include "../window.h"
#include "w_sdl3.h"


// Структура переменных окна:
typedef struct WindowSDL3_Vars {
    SDL_Window *window;
    SDL_GLContext gl_context;
    char title[1024];
    double start_time;
    double dtime;
    double dtime_old;
    bool   running;
    bool   focused;
    bool   defocused;
    bool   closing;
} WindowSDL3_Vars;


// Объявление функций:
static void WindowSDL3_RegisterAPI(Window *window);
static void WindowSDL3_MainLoop(Window *self, WinConfig *cfg);
static void WindowSDL3_Log_err(const char *msg, ...);
static void WindowSDL3_Closing_stage(Window *self);
static inline WindowSDL3_Vars* WindowSDL3_GetVars(Window *self);

static bool WindowSDL3_Impl_create(Window* self);
static bool WindowSDL3_Impl_close(Window *self);
static bool WindowSDL3_Impl_quit(Window *self);
static void WindowSDL3_Impl_set_title(Window *self, const char *title, ...);
static const char* WindowSDL3_Impl_get_title(Window *self);
static void WindowSDL3_Impl_set_icon(Window *self, Image *image);
static Image* WindowSDL3_Impl_get_icon(Window *self);
static void WindowSDL3_Impl_set_size(Window *self, int width, int height);
static void WindowSDL3_Impl_get_size(Window *self, int *width, int *height);
static void WindowSDL3_Impl_set_width(Window *self, int width);
static int WindowSDL3_Impl_get_width(Window *self);
static void WindowSDL3_Impl_set_height(Window *self, int height);
static int WindowSDL3_Impl_get_height(Window *self);
static void WindowSDL3_Impl_get_center(Window *self, int *x, int *y);
static void WindowSDL3_Impl_set_position(Window *self, int x, int y);
static void WindowSDL3_Impl_get_position(Window *self, int *x, int *y);
static void WindowSDL3_Impl_set_vsync(Window *self, bool vsync);
static bool WindowSDL3_Impl_get_vsync(Window *self);
static void WindowSDL3_Impl_set_fps(Window *self, int fps);
static int WindowSDL3_Impl_get_target_fps(Window *self);
static void WindowSDL3_Impl_set_visible(Window *self, bool visible);
static bool WindowSDL3_Impl_get_visible(Window *self);
static void WindowSDL3_Impl_set_titlebar(Window *self, bool titlebar);
static bool WindowSDL3_Impl_get_titlebar(Window *self);
static void WindowSDL3_Impl_set_resizable(Window *self, bool resizable);
static bool WindowSDL3_Impl_get_resizable(Window *self);
static void WindowSDL3_Impl_set_fullscreen(Window *self, bool fullscreen);
static bool WindowSDL3_Impl_get_fullscreen(Window *self);
static void WindowSDL3_Impl_set_min_size(Window *self, int width, int height);
static void WindowSDL3_Impl_get_min_size(Window *self, int *width, int *height);
static void WindowSDL3_Impl_set_max_size(Window *self, int width, int height);
static void WindowSDL3_Impl_get_max_size(Window *self, int *width, int *height);
static void WindowSDL3_Impl_set_always_top(Window *self, bool on_top);
static bool WindowSDL3_Impl_get_always_top(Window *self);
static bool WindowSDL3_Impl_get_is_focused(Window *self);
static bool WindowSDL3_Impl_get_is_defocused(Window *self);
static uint32_t WindowSDL3_Impl_get_window_display_id(Window *self);
static bool WindowSDL3_Impl_get_display_size(Window *self, uint32_t id, int *width, int *height);
static void WindowSDL3_Impl_maximize(Window *self);
static void WindowSDL3_Impl_minimize(Window *self);
static void WindowSDL3_Impl_restore(Window *self);
static void WindowSDL3_Impl_raise(Window *self);
static float WindowSDL3_Impl_get_current_fps(Window *self);
static double WindowSDL3_Impl_get_dtime(Window *self);
static double WindowSDL3_Impl_get_time(Window *self);
static void WindowSDL3_Impl_display(Window *self);

static Input_Scancode WindowSDL3_convert_scancode(SDL_Scancode scancode);
static void WindowSDL3_Impl_set_mouse_pos(Window *self, int x, int y);
static void WindowSDL3_Impl_set_mouse_visible(Window *self, bool visible);


// Регистрируем функции реализации апи:
static void WindowSDL3_RegisterAPI(Window *window) {
    window->create = WindowSDL3_Impl_create;
    window->close = WindowSDL3_Impl_close;
    window->quit = WindowSDL3_Impl_quit;
    window->set_title = WindowSDL3_Impl_set_title;
    window->get_title = WindowSDL3_Impl_get_title;
    window->set_icon = WindowSDL3_Impl_set_icon;
    window->get_icon = WindowSDL3_Impl_get_icon;
    window->set_size = WindowSDL3_Impl_set_size;
    window->get_size = WindowSDL3_Impl_get_size;
    window->set_width = WindowSDL3_Impl_set_width;
    window->get_width = WindowSDL3_Impl_get_width;
    window->set_height = WindowSDL3_Impl_set_height;
    window->get_height = WindowSDL3_Impl_get_height;
    window->get_center = WindowSDL3_Impl_get_center;
    window->set_position = WindowSDL3_Impl_set_position;
    window->get_position = WindowSDL3_Impl_get_position;
    window->set_vsync = WindowSDL3_Impl_set_vsync;
    window->get_vsync = WindowSDL3_Impl_get_vsync;
    window->set_fps = WindowSDL3_Impl_set_fps;
    window->get_target_fps = WindowSDL3_Impl_get_target_fps;
    window->set_visible = WindowSDL3_Impl_set_visible;
    window->get_visible = WindowSDL3_Impl_get_visible;
    window->set_titlebar = WindowSDL3_Impl_set_titlebar;
    window->get_titlebar = WindowSDL3_Impl_get_titlebar;
    window->set_resizable = WindowSDL3_Impl_set_resizable;
    window->get_resizable = WindowSDL3_Impl_get_resizable;
    window->set_fullscreen = WindowSDL3_Impl_set_fullscreen;
    window->get_fullscreen = WindowSDL3_Impl_get_fullscreen;
    window->set_min_size = WindowSDL3_Impl_set_min_size;
    window->get_min_size = WindowSDL3_Impl_get_min_size;
    window->set_max_size = WindowSDL3_Impl_set_max_size;
    window->get_max_size = WindowSDL3_Impl_get_max_size;
    window->set_always_top = WindowSDL3_Impl_set_always_top;
    window->get_always_top = WindowSDL3_Impl_get_always_top;
    window->get_is_focused = WindowSDL3_Impl_get_is_focused;
    window->get_is_defocused = WindowSDL3_Impl_get_is_defocused;
    window->get_window_display_id = WindowSDL3_Impl_get_window_display_id;
    window->get_display_size = WindowSDL3_Impl_get_display_size;
    window->maximize = WindowSDL3_Impl_maximize;
    window->minimize = WindowSDL3_Impl_minimize;
    window->restore = WindowSDL3_Impl_restore;
    window->raise = WindowSDL3_Impl_raise;
    window->get_current_fps = WindowSDL3_Impl_get_current_fps;
    window->get_dtime = WindowSDL3_Impl_get_dtime;
    window->get_time = WindowSDL3_Impl_get_time;
    window->display = WindowSDL3_Impl_display;
}


// Создать окно:
Window* WindowSDL3_create(WinConfig *config, Renderer *renderer) {
    Window *window = (Window*)mm_alloc(sizeof(Window));
    if (!window) mm_alloc_error();

    // Создаём локальные переменные окна:
    WindowSDL3_Vars *winvars = (WindowSDL3_Vars*)mm_calloc(1, sizeof(WindowSDL3_Vars));
    if (!winvars) mm_alloc_error();

    // Создаём систему ввода:
    Input *input = Input_create(
        Input_MouseState_create(8),
        Input_KeyboardState_create(INPUT_SCANCODE_COUNT),
        WindowSDL3_Impl_set_mouse_pos,
        WindowSDL3_Impl_set_mouse_visible
    );

    // Сохраняем указатели:
    window->config = config;
    window->renderer = renderer;
    window->input = input;
    window->data = winvars;

    // Регистрируем функции:
    WindowSDL3_RegisterAPI(window);

    return window;
}


// Уничтожить окно:
void WindowSDL3_destroy(Window **window) {
    if (!window || !*window) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(*window);

    // Вызываем закрытие окна, если оно ещё не закрыто:
    if (WinVars->window) {
        (*window)->close(*window);
        (*window)->quit(*window);
    }

    // Освобождаем память системы ввода:
    if ((*window)->input) {
        Input_MouseState_destroy(&(*window)->input->mouse);
        Input_KeyboardState_destroy(&(*window)->input->keyboard);
        Input_destroy(&(*window)->input);
    }

    // Освобождаем память глобальных переменных:
    if (WinVars) {
        mm_free(WinVars);
        WinVars = NULL;
    }

    // Освободить память окна:
    mm_free(*window);
    *window = NULL;
}


// Главный цикл окна (ядро окна. Запускается из WindowSDL3_Impl_create):
static void WindowSDL3_MainLoop(Window *self, WinConfig *cfg) {
    if (!self || !cfg) {
        WindowSDL3_Log_err("WSDL3-FAIL: MainLoop not be started (arguments are invalid).\n");
        return;
    }

    // Получаем глобальные переменные окна:
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    // Вызываем старт:
    if (cfg->start) cfg->start(self);

    // Основной цикл окна:
    WinVars->running = true;
    while (WinVars->running) {
        // Настраиваем переменные:
        double start_frame_time = self->get_time(self);
        WinVars->focused = false;
        WinVars->defocused = false;

        // Проверяем чтобы дельта времени не была равна нулю. Иначе используем прошлую дельту времени:
        if (WinVars->dtime > 0.0) { WinVars->dtime_old = WinVars->dtime; }
        else { WinVars->dtime = WinVars->dtime_old; }

        // Получаем копию указателя на систему ввода:
        Input *input = self->input;

        // Сброс состояний клавиатуры и мыши:
        input->mouse->rel = (Vec2i){0, 0};
        input->mouse->wheel = (Vec2i){0, 0};
        memset(input->mouse->down, 0, input->mouse->max_keys * sizeof(bool));
        memset(input->mouse->up,   0, input->mouse->max_keys * sizeof(bool));
        memset(input->keyboard->down, 0, input->keyboard->max_keys * sizeof(bool));
        memset(input->keyboard->up,   0, input->keyboard->max_keys * sizeof(bool));

        // Обрабатываем события:
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                // Если окно хотят закрыть:
                case SDL_EVENT_QUIT:
                    self->close(self);
                    break;

                // Если размер окна изменился:
                case SDL_EVENT_WINDOW_RESIZED:
                    if (cfg->resize) cfg->resize(self, event.window.data1, event.window.data2);
                    break;

                // Окно развернули:
                case SDL_EVENT_WINDOW_RESTORED:
                    if (cfg->show) cfg->show(self);
                    break;

                // Окно свернули:
                case SDL_EVENT_WINDOW_MINIMIZED:
                    if (cfg->hide) cfg->hide(self);
                    break;

                // Окно стало активным:
                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    WinVars->focused = true;
                    break;

                // Окно потеряло фокус:
                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    WinVars->defocused = true;
                    break;

                // Обработка ввода:

                // Если мышь зашла в окно:
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                    input->mouse->focused = true;
                    break;

                // Если мышь покинула окно:
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                    input->mouse->focused = false;
                    break;

                // Если мышь передвинулась:
                case SDL_EVENT_MOUSE_MOTION:
                    input->mouse->rel.x = event.motion.xrel;
                    input->mouse->rel.y = event.motion.yrel;
                    input->mouse->pos.x = event.motion.x;
                    input->mouse->pos.y = event.motion.y;
                    break;

                // Если колёсико мыши провернулось:
                case SDL_EVENT_MOUSE_WHEEL:
                    input->mouse->wheel.x = event.wheel.x;
                    input->mouse->wheel.y = event.wheel.y;
                    break;

                // Если нажимают кнопку мыши:
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button < input->mouse->max_keys) {
                        input->mouse->pressed[event.button.button - 1] = true;
                        input->mouse->down[event.button.button - 1] = true;
                    }
                    break;

                // Если отпускают кнопку мыши:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button < input->mouse->max_keys) {
                        input->mouse->pressed[event.button.button - 1] = false;
                        input->mouse->up[event.button.button - 1] = true;
                    }
                    break;

                // Если нажимают кнопку на клавиатуре:
                case SDL_EVENT_KEY_DOWN: {
                    Input_Scancode scancode = WindowSDL3_convert_scancode(event.key.scancode);
                    if (scancode < input->keyboard->max_keys) {
                        if (!input->keyboard->pressed[scancode]) {
                            input->keyboard->pressed[scancode] = true;
                            input->keyboard->down[scancode] = true;
                        }
                    }
                    break;
                }

                // Если отпускают кнопку на клавиатуре:
                case SDL_EVENT_KEY_UP: {
                    Input_Scancode scancode = WindowSDL3_convert_scancode(event.key.scancode);
                    if (scancode < input->keyboard->max_keys) {
                        input->keyboard->pressed[scancode] = false;
                        input->keyboard->up[scancode] = true;
                    }
                    break;
                }
            }
        }

        // Обработка основных функций (обновление и отрисовка):
        if (cfg->update) cfg->update(self, self->input, self->get_dtime(self));
        if (cfg->render) cfg->render(self, self->renderer, self->get_dtime(self));

        // Проверяем что окно хотят закрыть:
        if (WinVars->closing) {
            WindowSDL3_Closing_stage(self);
            return;
        }

        // Делаем задержку между кадрами:
        if (!self->config->vsync && cfg->fps > 0) {
            SDL_Delay(1000 / cfg->fps);  // Фиксированный FPS (значения в аргументах в мс в целых числах).
        } else SDL_Delay(0);  // При vsync можно не задерживать — SDL будет синхронизировать кадры.

        // Получаем дельту времени (время кадра или же время обработки одного цикла окна):
        WinVars->dtime = self->get_time(self) - start_frame_time;
    }

    self->close(self);
}


// Логирование ошибок:
static void WindowSDL3_Log_err(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
    va_end(args);
}


static void WindowSDL3_Closing_stage(Window *self) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    // Вызываем уничтожение:
    if (self->config->destroy) {
        self->config->destroy(self);
    }

    // Уничтожаем контекст рендеринга:
    switch (self->renderer->type) {
        case RENDERER_OPENGL:
            if (WinVars->gl_context) { SDL_GL_DestroyContext(WinVars->gl_context); }
            WinVars->gl_context = NULL;
            break;

        // Other renderers.
    }

    // Уничтожаем окно:
    SDL_DestroyWindow(WinVars->window);
    memset(WinVars, 0, sizeof(WindowSDL3_Vars));
}


// Получение переменных окна:
static inline WindowSDL3_Vars* WindowSDL3_GetVars(Window *self) {
    return (WindowSDL3_Vars*)self->data;
}


// Реализация API:


static bool WindowSDL3_Impl_create(Window *self) {
    if (!self || !self->config || !self->renderer) return false;
    WinConfig *cfg = self->config;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return false;

    // Инициализируем SDL:
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        WindowSDL3_Log_err("WSDL3-FAIL: SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }

    // Флаги окна:
    SDL_WindowFlags flags = SDL_WINDOW_HIDDEN;

    // Настраиваем атрибуты рендерера:
    switch (self->renderer->type) {
        case RENDERER_OPENGL: {
            RendererGL_Data *rnd_data = (RendererGL_Data*)self->renderer->data;
            // Настраиваем профиль:
            uint32_t profile = SDL_GL_CONTEXT_PROFILE_CORE;  // По умолчанию.
            if (rnd_data->profile == RENDERER_GL_CORE) {
                profile = SDL_GL_CONTEXT_PROFILE_CORE;
            } else if (rnd_data->profile == RENDERER_GL_COMPATIBILITY) {
                profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
            }
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, rnd_data->major);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, rnd_data->minor);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, rnd_data->doublebuffer ? 1 : 0);
            flags |= SDL_WINDOW_OPENGL;
            break;
        }

        // Other renderers.

        default:
            WindowSDL3_Log_err("WSDL3-FAIL: Renderer \"%s\" not supported.\n", self->renderer->name);
            SDL_Quit();
            return false;
    }

    // Инициализируем окно:
    SDL_Window *window = SDL_CreateWindow(cfg->title, cfg->width, cfg->height, flags);
    if (!window) {
        WindowSDL3_Log_err("WSDL3-FAIL: SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Создаём контекст рендерера:
    switch (self->renderer->type) {
        case RENDERER_OPENGL:
            WinVars->gl_context = SDL_GL_CreateContext(window);
            if (!WinVars->gl_context) {
                WindowSDL3_Log_err("WSDL3-FAIL: Creating context for renderer \"%s\" failed.\n", self->renderer->name);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return false;
            }
            SDL_GL_MakeCurrent(window, WinVars->gl_context);  // Делаем контекст активным.
            SDL_GL_SetSwapInterval(0);  // По умолчанию, отключаем VSync.

            // Инициализируем рендерер:
            self->renderer->init(self->renderer);
            break;

        // Other renderers.

        default:
            WindowSDL3_Log_err("WSDL3-FAIL: Context for renderer \"%s\" not found.\n", self->renderer->name);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
    }

    // Устанавливаем значения в глобальные переменные:
    WinVars->window = window;
    WinVars->start_time = self->get_time(self);
    WinVars->dtime = cfg->fps <= 0 ? 1.0/60.0 : 1.0/cfg->fps;
    WinVars->dtime_old = WinVars->dtime;
    WinVars->closing = false;

    // Настройка окна:
    self->set_vsync(self, cfg->vsync);
    self->set_fps(self, cfg->fps);
    self->set_position(self, cfg->x, cfg->y);
    self->set_titlebar(self, cfg->titlebar);
    self->set_resizable(self, cfg->resizable);
    self->set_fullscreen(self, cfg->fullscreen);
    self->set_min_size(self, cfg->min_width, cfg->min_height);
    self->set_max_size(self, cfg->max_width, cfg->max_height);
    self->set_always_top(self, cfg->always_top);
    self->set_visible(self, cfg->visible);  // Применяем видимость только после применения других настроек.

    // Запускаем главный цикл:
    WindowSDL3_MainLoop(self, cfg);

    return true;
}


static bool WindowSDL3_Impl_close(Window *self) {
    if (!self || !self->config) return false;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return false;

    WinVars->closing = true;
    return true;
}


static bool WindowSDL3_Impl_quit(Window *self) {
    if (!self) return false;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);

    // Останавливаем все окна и закрываем SDL:
    if (WinVars && WinVars->window) WindowSDL3_Impl_close(self);
    SDL_Quit();

    return true;
}


static void WindowSDL3_Impl_set_title(Window *self, const char *title, ...) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    va_list args;
    va_start(args, title);
    vsnprintf(WinVars->title, sizeof(WinVars->title), title, args);
    va_end(args);
    SDL_SetWindowTitle(WinVars->window, WinVars->title);
    self->config->title = WinVars->title;
}


static const char* WindowSDL3_Impl_get_title(Window *self) {
    if (!self) return NULL;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return NULL;
    return SDL_GetWindowTitle(WinVars->window);
}


static void WindowSDL3_Impl_set_icon(Window *self, Image *image) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    if (self->config->icon) Image_destroy(&self->config->icon);
    self->config->icon = Image_copy(image);

    if (!image) {
        SDL_Surface *empty_icon = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
        SDL_memset(empty_icon->pixels, 0, empty_icon->h * empty_icon->pitch);
        if (empty_icon) {
            SDL_SetWindowIcon(WinVars->window, empty_icon);
            SDL_DestroySurface(empty_icon);
        }
    } else {
        SDL_Surface *icon = SDL_CreateSurfaceFrom(
            image->width, image->height, SDL_PIXELFORMAT_RGBA32,
            image->data, image->width * image->channels);
        if (icon) {
            SDL_SetWindowIcon(WinVars->window, icon);
            SDL_DestroySurface(icon);
        }
    }
}


static Image* WindowSDL3_Impl_get_icon(Window *self) {
    if (!self || !self->config) return NULL;
    return self->config->icon;
}


static void WindowSDL3_Impl_set_size(Window *self, int width, int height) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowSize(WinVars->window, width, height);
    self->config->width = width;
    self->config->height = height;
}


static void WindowSDL3_Impl_get_size(Window *self, int *width, int *height) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_GetWindowSize(WinVars->window, width, height);
}


static void WindowSDL3_Impl_set_width(Window *self, int width) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowSize(WinVars->window, width, self->config->height);
    self->config->width = width;
}


static int WindowSDL3_Impl_get_width(Window *self) {
    if (!self) return 0;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return 0;
    int width;
    SDL_GetWindowSize(WinVars->window, &width, NULL);
    return width;
}


static void WindowSDL3_Impl_set_height(Window *self, int height) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowSize(WinVars->window, self->config->width, height);
    self->config->height = height;
}


static int WindowSDL3_Impl_get_height(Window *self) {
    if (!self) return 0;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return 0;
    int height;
    SDL_GetWindowSize(WinVars->window, NULL, &height);
    return height;
}


static void WindowSDL3_Impl_get_center(Window *self, int *x, int *y) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    int w, h;
    SDL_GetWindowSize(WinVars->window, &w, &h);
    *x = w / 2;
    *y = h / 2;
}


static void WindowSDL3_Impl_set_position(Window *self, int x, int y) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    // Обрабатываем позицию окна (координаты -1 означают выравнивание по центру экрана):
    int pos_x = x, pos_y = y;
    if (x == -1 || y == -1) {
        int ww, wh, dw, dh;
        self->get_size(self, &ww, &wh);
        self->get_display_size(self, self->get_window_display_id(self), &dw, &dh);
        if (x == -1) pos_x = (dw - ww) / 2;
        if (y == -1) pos_y = (dh - wh) / 2;
    }

    SDL_SetWindowPosition(WinVars->window, pos_x, pos_y);
}


static void WindowSDL3_Impl_get_position(Window *self, int *x, int *y) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_GetWindowPosition(WinVars->window, x, y);
}


static void WindowSDL3_Impl_set_vsync(Window *self, bool vsync) {
    if (!self || !self->config) return;

    switch (self->renderer->type) {
        case RENDERER_OPENGL:
            SDL_GL_SetSwapInterval(vsync);
            break;

        // Other renderers.
    }

    self->config->vsync = vsync;
}


static bool WindowSDL3_Impl_get_vsync(Window *self) {
    if (!self || !self->config) return false;
    return self->config->vsync;
}


static void WindowSDL3_Impl_set_fps(Window *self, int fps) {
    if (!self || !self->config) return;
    self->config->fps = fps;
}


static int WindowSDL3_Impl_get_target_fps(Window *self) {
    if (!self || !self->config) return 0;
    return self->config->fps;
}


static void WindowSDL3_Impl_set_visible(Window *self, bool visible) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    if (visible) { SDL_ShowWindow(WinVars->window); }
    else { SDL_HideWindow(WinVars->window); }
    self->config->visible = visible;
}


static bool WindowSDL3_Impl_get_visible(Window *self) {
    if (!self || !self->config) return false;
    return self->config->visible;
}


static void WindowSDL3_Impl_set_titlebar(Window *self, bool titlebar) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowBordered(WinVars->window, titlebar);
    self->config->titlebar = titlebar;
}


static bool WindowSDL3_Impl_get_titlebar(Window *self) {
    if (!self || !self->config) return false;
    return self->config->titlebar;
}


static void WindowSDL3_Impl_set_resizable(Window *self, bool resizable) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowResizable(WinVars->window, resizable);
    self->config->resizable = resizable;
}


static bool WindowSDL3_Impl_get_resizable(Window *self) {
    if (!self || !self->config) return false;
    return self->config->resizable;
}


static void WindowSDL3_Impl_set_fullscreen(Window *self, bool fullscreen) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowFullscreen(WinVars->window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    self->config->fullscreen = fullscreen;
}


static bool WindowSDL3_Impl_get_fullscreen(Window *self) {
    if (!self || !self->config) return false;
    return self->config->fullscreen;
}


static void WindowSDL3_Impl_set_min_size(Window *self, int width, int height) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowMinimumSize(WinVars->window, width, height);
}


static void WindowSDL3_Impl_get_min_size(Window *self, int *width, int *height) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_GetWindowMinimumSize(WinVars->window, width, height);
}


static void WindowSDL3_Impl_set_max_size(Window *self, int width, int height) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowMaximumSize(WinVars->window, width, height);
}


static void WindowSDL3_Impl_get_max_size(Window *self, int *width, int *height) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_GetWindowMaximumSize(WinVars->window, width, height);
}


static void WindowSDL3_Impl_set_always_top(Window *self, bool on_top) {
    if (!self || !self->config) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_SetWindowAlwaysOnTop(WinVars->window, on_top);
    self->config->always_top = on_top;
}


static bool WindowSDL3_Impl_get_always_top(Window *self) {
    if (!self || !self->config) return false;
    return self->config->always_top;
}


static bool WindowSDL3_Impl_get_is_focused(Window *self) {
    if (!self) return false;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return false;
    return WinVars->focused;
}


static bool WindowSDL3_Impl_get_is_defocused(Window *self) {
    if (!self) return false;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return false;
    return WinVars->defocused;
}


static uint32_t WindowSDL3_Impl_get_window_display_id(Window *self) {
    if (!self) return 0;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return 0;
    return (uint32_t)SDL_GetDisplayForWindow(WinVars->window);
}


static bool WindowSDL3_Impl_get_display_size(Window *self, uint32_t id, int *width, int *height) {
    if (!self) return false;
    SDL_Rect rect = { 0, 0, 0, 0 };
    if (!SDL_GetDisplayUsableBounds((SDL_DisplayID)id, &rect)) return false;
    *width = rect.w;
    *height = rect.h;
    return true;
}


static void WindowSDL3_Impl_maximize(Window *self) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_MaximizeWindow(WinVars->window);
}


static void WindowSDL3_Impl_minimize(Window *self) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_MinimizeWindow(WinVars->window);
}


static void WindowSDL3_Impl_restore(Window *self) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_RestoreWindow(WinVars->window);
}


static void WindowSDL3_Impl_raise(Window *self) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_RaiseWindow(WinVars->window);
}


static float WindowSDL3_Impl_get_current_fps(Window *self) {
    if (!self) return 0.0f;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return 0.0f;
    return (float)(1.0/WinVars->dtime);
}


static double WindowSDL3_Impl_get_dtime(Window *self) {
    if (!self) return 0.0f;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return 0.0f;
    return WinVars->dtime;
}


static double WindowSDL3_Impl_get_time(Window *self) {
    if (!self) return 0.0;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars) return 0.0;
    uint64_t counter = SDL_GetPerformanceCounter();  // Получаем высокоточный счётчик процессора.
    uint64_t freq = SDL_GetPerformanceFrequency();   // Получаем количество тиков в секунду.
    double time = (double)counter / (double)freq;    // Получаем время в секундах с высоким разрешением.
    return time - WinVars->start_time;               // Получаем время с начала создания окна.
}


static void WindowSDL3_Impl_display(Window *self) {
    if (!self) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;

    switch (self->renderer->type) {
        case RENDERER_OPENGL:
            SDL_GL_SwapWindow(WinVars->window);
            break;

        // Other renderers.
    }
}


// Реализация функций ввода:


static Input_Scancode WindowSDL3_convert_scancode(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_0: return K_0;
        case SDL_SCANCODE_1: return K_1;
        case SDL_SCANCODE_2: return K_2;
        case SDL_SCANCODE_3: return K_3;
        case SDL_SCANCODE_4: return K_4;
        case SDL_SCANCODE_5: return K_5;
        case SDL_SCANCODE_6: return K_6;
        case SDL_SCANCODE_7: return K_7;
        case SDL_SCANCODE_8: return K_8;
        case SDL_SCANCODE_9: return K_9;
        case SDL_SCANCODE_A: return K_a;
        case SDL_SCANCODE_B: return K_b;
        case SDL_SCANCODE_C: return K_c;
        case SDL_SCANCODE_D: return K_d;
        case SDL_SCANCODE_E: return K_e;
        case SDL_SCANCODE_F: return K_f;
        case SDL_SCANCODE_G: return K_g;
        case SDL_SCANCODE_H: return K_h;
        case SDL_SCANCODE_I: return K_i;
        case SDL_SCANCODE_J: return K_j;
        case SDL_SCANCODE_K: return K_k;
        case SDL_SCANCODE_L: return K_l;
        case SDL_SCANCODE_M: return K_m;
        case SDL_SCANCODE_N: return K_n;
        case SDL_SCANCODE_O: return K_o;
        case SDL_SCANCODE_P: return K_p;
        case SDL_SCANCODE_Q: return K_q;
        case SDL_SCANCODE_R: return K_r;
        case SDL_SCANCODE_S: return K_s;
        case SDL_SCANCODE_T: return K_t;
        case SDL_SCANCODE_U: return K_u;
        case SDL_SCANCODE_V: return K_v;
        case SDL_SCANCODE_W: return K_w;
        case SDL_SCANCODE_X: return K_x;
        case SDL_SCANCODE_Y: return K_y;
        case SDL_SCANCODE_Z: return K_z;

        case SDL_SCANCODE_KP_0: return K_KP_0;
        case SDL_SCANCODE_KP_1: return K_KP_1;
        case SDL_SCANCODE_KP_2: return K_KP_2;
        case SDL_SCANCODE_KP_3: return K_KP_3;
        case SDL_SCANCODE_KP_4: return K_KP_4;
        case SDL_SCANCODE_KP_5: return K_KP_5;
        case SDL_SCANCODE_KP_6: return K_KP_6;
        case SDL_SCANCODE_KP_7: return K_KP_7;
        case SDL_SCANCODE_KP_8: return K_KP_8;
        case SDL_SCANCODE_KP_9: return K_KP_9;
        case SDL_SCANCODE_KP_EXCLAM: return K_KP_EXCLAIM;
        case SDL_SCANCODE_KP_HASH: return K_KP_HASH;
        case SDL_SCANCODE_KP_AMPERSAND: return K_KP_AMPERSAND;
        case SDL_SCANCODE_KP_LEFTPAREN: return K_KP_LEFTPAREN;
        case SDL_SCANCODE_KP_RIGHTPAREN: return K_KP_RIGHTPAREN;
        case SDL_SCANCODE_KP_PERIOD: return K_KP_PERIOD;
        case SDL_SCANCODE_KP_DIVIDE: return K_KP_DIVIDE;
        case SDL_SCANCODE_KP_MULTIPLY: return K_KP_MULTIPLY;
        case SDL_SCANCODE_KP_MINUS: return K_KP_MINUS;
        case SDL_SCANCODE_KP_PLUS: return K_KP_PLUS;
        case SDL_SCANCODE_KP_ENTER: return K_KP_ENTER;
        case SDL_SCANCODE_KP_EQUALS: return K_KP_EQUALS;
        case SDL_SCANCODE_KP_COLON: return K_KP_COLON;
        case SDL_SCANCODE_KP_LESS: return K_KP_LESS;
        case SDL_SCANCODE_KP_GREATER: return K_KP_GREATER;
        case SDL_SCANCODE_KP_AT: return K_KP_AT;

        case SDL_SCANCODE_F1: return K_F1;
        case SDL_SCANCODE_F2: return K_F2;
        case SDL_SCANCODE_F3: return K_F3;
        case SDL_SCANCODE_F4: return K_F4;
        case SDL_SCANCODE_F5: return K_F5;
        case SDL_SCANCODE_F6: return K_F6;
        case SDL_SCANCODE_F7: return K_F7;
        case SDL_SCANCODE_F8: return K_F8;
        case SDL_SCANCODE_F9: return K_F9;
        case SDL_SCANCODE_F10: return K_F10;
        case SDL_SCANCODE_F11: return K_F11;
        case SDL_SCANCODE_F12: return K_F12;
        case SDL_SCANCODE_F13: return K_F13;
        case SDL_SCANCODE_F14: return K_F14;
        case SDL_SCANCODE_F15: return K_F15;

        case SDL_SCANCODE_BACKSPACE: return K_BACKSPACE;
        case SDL_SCANCODE_TAB: return K_TAB;
        case SDL_SCANCODE_CLEAR: return K_CLEAR;
        case SDL_SCANCODE_RETURN: return K_RETURN;
        case SDL_SCANCODE_PAUSE: return K_PAUSE;
        case SDL_SCANCODE_ESCAPE: return K_ESCAPE;
        case SDL_SCANCODE_SPACE: return K_SPACE;
        case SDL_SCANCODE_COMMA: return K_COMMA;
        case SDL_SCANCODE_MINUS: return K_MINUS;
        case SDL_SCANCODE_PERIOD: return K_PERIOD;
        case SDL_SCANCODE_SLASH: return K_SLASH;
        case SDL_SCANCODE_SEMICOLON: return K_SEMICOLON;
        case SDL_SCANCODE_EQUALS: return K_EQUALS;
        case SDL_SCANCODE_LEFTBRACKET: return K_LEFTBRACKET;
        case SDL_SCANCODE_BACKSLASH: return K_BACKSLASH;
        case SDL_SCANCODE_RIGHTBRACKET: return K_RIGHTBRACKET;
        case SDL_SCANCODE_DELETE: return K_DELETE;
        case SDL_SCANCODE_UP: return K_UP;
        case SDL_SCANCODE_DOWN: return K_DOWN;
        case SDL_SCANCODE_RIGHT: return K_RIGHT;
        case SDL_SCANCODE_LEFT: return K_LEFT;
        case SDL_SCANCODE_INSERT: return K_INSERT;
        case SDL_SCANCODE_HOME: return K_HOME;
        case SDL_SCANCODE_END: return K_END;
        case SDL_SCANCODE_PAGEUP: return K_PAGEUP;
        case SDL_SCANCODE_PAGEDOWN: return K_PAGEDOWN;
        case SDL_SCANCODE_NUMLOCKCLEAR: return K_NUMLOCK;
        case SDL_SCANCODE_CAPSLOCK: return K_CAPSLOCK;
        case SDL_SCANCODE_SCROLLLOCK: return K_SCROLLLOCK;
        case SDL_SCANCODE_RSHIFT: return K_RSHIFT;
        case SDL_SCANCODE_LSHIFT: return K_LSHIFT;
        case SDL_SCANCODE_RCTRL: return K_RCTRL;
        case SDL_SCANCODE_LCTRL: return K_LCTRL;
        case SDL_SCANCODE_RALT: return K_RALT;
        case SDL_SCANCODE_LALT: return K_LALT;
        case SDL_SCANCODE_RGUI: return K_RGUI;
        case SDL_SCANCODE_LGUI: return K_LGUI;
        case SDL_SCANCODE_MODE: return K_MODE;
        case SDL_SCANCODE_HELP: return K_HELP;
        case SDL_SCANCODE_PRINTSCREEN: return K_PRINTSCREEN;
        case SDL_SCANCODE_SYSREQ: return K_SYSREQ;
        case SDL_SCANCODE_MENU: return K_MENU;
        case SDL_SCANCODE_POWER: return K_POWER;

        // Other keys...

        case SDL_SCANCODE_UNKNOWN:
        default: return K_UNKNOWN;
    }
}


static void WindowSDL3_Impl_set_mouse_pos(Window *self, int x, int y) {
    if (!self || !self->input || !self->input->keyboard) return;
    WindowSDL3_Vars *WinVars = WindowSDL3_GetVars(self);
    if (!WinVars || !WinVars->window) return;
    SDL_WarpMouseInWindow(WinVars->window, x, y);
}


static void WindowSDL3_Impl_set_mouse_visible(Window *self, bool visible) {
    if (!self || !self->input || !self->input->keyboard) return;
    visible ? SDL_ShowCursor() : SDL_HideCursor();
    self->input->mouse->visible = visible;
}
