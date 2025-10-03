//
// main.c - Основной файл программы.
//


// Подключаем:
#include <stdio.h>
#include <stdlib.h>
#include <engine/engine.h>
#include <engine/core/graphics/gl.h>
#include <cglm/cglm.h>
#include <math.h>


// Вызывается после создания окна:
void start(Window *self) {
    printf("Start called.\n");

    Image *image = Image_load("data/no_icon.png", IMG_RGBA);
    if (!image) {
        printf("Image not found. Using default.\n");
        image = Image_create_default();
    }
    self->set_icon(self, image);
    Image_destroy(&image);
    self->set_fps(self, 60);
    self->set_vsync(self, false);
}


// Вызывается каждый кадр (цикл окна):
void update(Window *self, Input *input, float dt) {
    printf("Update called. FPS %f\n", self->get_current_fps(self));

    // ...

    if (input->get_key_up(self)[K_f]) self->set_fullscreen(self, !self->get_fullscreen(self));
    if (input->get_key_up(self)[K_ESCAPE]) self->quit(self);
}


// Вызывается каждый кадр (отрисовка окна):
void render(Window *self, Renderer *render, float dt) {
    float t = self->get_time(self);
    float r = 0.5 + 0.5 * cosf(t + 0.0);
    float g = 0.5 + 0.5 * cosf(t + 2.0);
    float b = 0.5 + 0.5 * cosf(t + 4.0);
    render->clear(render, r, g, b, 1.0f);

    // ...

    self->display(self);
}


// Вызывается при изменении размера окна:
void resize(Window *self, int width, int height) {
    printf("ReSize called. New W: %d, H: %d\n", width, height);
}


// Вызывается при разворачивании окна:
void show(Window *self) {
    printf("Show called.\n");
}


// Вызывается при сворачивании окна:
void hide(Window *self) {
    printf("Hide called.\n");
}


// Вызывается при закрытии окна:
void destroy(Window *self) {
    printf("Destroy called.\n");
}


// Точка входа в программу:
int main(int argc, char *argv[]) {
    printf("Engine version: %s\n", ENGINE_VERSION);

    Renderer *renderer = RendererGL_create(4, 1, true, RENDERER_GL_CORE);
    WinConfig *config = Window_create_config(start, update, render, resize, show, hide, destroy);
    Window *window = WindowSDL3_create(config, renderer);
    window->create(window);

    printf("\n(Before free) Memory used: %g kb (%zu b).\n", mm_get_used_size_kb(), mm_get_used_size());

    WindowSDL3_destroy(&window);
    Window_destroy_config(&config);
    RendererGL_destroy(&renderer);

    printf("(After free) Memory used: %g kb (%zu b).\n", mm_get_used_size_kb(), mm_get_used_size());
    if (mm_get_used_size() > 0) printf("Memory leak!\n");

    return 0;
}
