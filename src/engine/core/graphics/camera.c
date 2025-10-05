//
// camera.c - Исходник, реализовывающий работу камеры.
//
// Тут используется обычный malloc/free, потому что mm движка не выравнивает указатели на блоки,
// из за чего происходит крах программы. TODO: Сделать чтобы в mm, возвращаемые указатели были
// выравнены по 16 байт.
//


// Подключаем:
#include <stdlib.h>
#include "../mm/mm.h"
#include "../math.h"
#include "renderer.h"
#include "window.h"
#include "camera.h"


// Объявление функций:
static void Camera2D_Impl_update(Camera2D *self);
static void Camera2D_Impl_resize(Camera2D *self, int width, int height);
static void Camera2D_Impl_ui_begin(Camera2D *self);
static void Camera2D_Impl_ui_end(Camera2D *self);


// Создать 2D камеру:
Camera2D* Camera2D_create(Window *window, int width, int height, Vec2d position, float angle, float zoom) {
    Camera2D *camera = (Camera2D*)malloc(sizeof(Camera2D));
    if (!camera) mm_alloc_error();
    else mm_used_size_add(sizeof(Camera2D));

    // Заполняем поля:
    camera->window = window;
    camera->position.x = position.x;
    camera->position.y = position.y;
    camera->angle = angle;
    camera->zoom = zoom;
    camera->meter = 100.0f;
    camera->width = width;
    camera->height = height;

    // Матрица вида:
    glm_mat4_identity(camera->view);
    glm_translate(camera->view, (vec3){-camera->position.x, -camera->position.y, 0.0f});
    glm_rotate(camera->view, glm_rad(camera->angle), (vec3){0, 0, 1.0f});
    glm_scale(camera->view, (vec3){1.0f/camera->zoom, 1.0f/camera->zoom, 1.0f});

    // // Матрица проекции:
    glm_mat4_identity(camera->proj);
    float wdth = camera->width/2.0f * camera->meter/100.0f;
    float hght = camera->height/2.0f * camera->meter/100.0f;
    glm_ortho(-wdth, wdth, -hght, hght, -1.0, 1.0, camera->proj);

    // Регистрируем функции:
    camera->update = Camera2D_Impl_update;
    camera->resize = Camera2D_Impl_resize;
    camera->ui_begin = Camera2D_Impl_ui_begin;
    camera->ui_end = Camera2D_Impl_ui_end;

    // Установка ортогональной проекции:
    camera->resize(camera, width, height);

    // Масштабирование и перемещение:
    camera->update(camera);

    return camera;
}


// Уничтожить 2D камеру:
void Camera2D_destroy(Camera2D **camera) {
    if (!camera || !*camera) return;
    free(*camera);
    mm_used_size_sub(sizeof(Camera2D));
    *camera = NULL;
}


// Реализация API:


static void Camera2D_Impl_update(Camera2D *self) {
    glm_mat4_identity(self->view);
    if (self->zoom != 0.0) {
        glm_scale(self->view, (vec3){1.0f/self->zoom, 1.0f/self->zoom, 1.0f});
    } else {
        glm_scale(self->view, (vec3){0.0f, 0.0f, 1.0f});
    }
    glm_rotate(self->view, glm_rad(self->angle), (vec3){0, 0, 1.0f});
    glm_translate(self->view, (vec3){-self->position.x, -self->position.y, 0.0f});

    // Устанавливаем активную камеру:
    self->window->renderer->camera = (void*)self;

    // Обновляем данные матриц в шейдере по умолчанию:
    self->window->renderer->camera2d_update(self->window->renderer);
}


static void Camera2D_Impl_resize(Camera2D *self, int width, int height) {
    self->width = width;
    self->height = height;
    self->window->renderer->viewport_resize(self->window->renderer, width, height);

    glm_mat4_identity(self->proj);
    float wdth = self->width/2.0f * self->meter/100.0f;
    float hght = self->height/2.0f * self->meter/100.0f;
    glm_ortho(-wdth, wdth, -hght, hght, -1.0, 1.0, self->proj);
    return;
}


static void Camera2D_Impl_ui_begin(Camera2D *self) {
    /*
    if self._ui_begin_:
        raise Exception(
            "Function \".ui_end()\" was not called in the last iteration of the loop.\n"
            "The function \".ui_begin()\" cannot be called, since the last one "
            "\".ui_begin()\" was not closed by the \".ui_end()\" function.")
    self._ui_begin_ = True
    if self.size.xy != self._old_size_.xy:
        self._ui_view_ = glm.translate(mat4(1.0), vec3(-self.size.xy/2, 0))
        self._old_size_ = self.size.xy
    # Обнуляем матрицу вида в шейдере по умолчанию:
    RenderPipeline.default_shader.begin()
    RenderPipeline.default_shader.set_uniform("u_view", self._ui_view_)
    RenderPipeline.default_shader.end()
    */
    return;
}


static void Camera2D_Impl_ui_end(Camera2D *self) {
    /*
    if self._ui_begin_: self._ui_begin_ = False
    else: raise Exception("The \".ui_begin()\" function was not called before the \".ui_end()\" function.")
    # Возвращаем обратно матрицу вида в шейдере по умолчанию:
    RenderPipeline.default_shader.begin()
    RenderPipeline.default_shader.set_uniform("u_view", self.view)
    RenderPipeline.default_shader.end()
    */
    return;
}
