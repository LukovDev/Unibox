//
// camera.c - Исходник, реализовывающий работу камеры.
//


// Подключаем:
#include <stdlib.h>
#include "../mm/mm.h"
#include "../math.h"
#include "shader.h"
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
    Camera2D *camera = (Camera2D*)mm_alloc(sizeof(Camera2D));
    if (!camera) mm_alloc_error();

    // Заполняем поля:
    camera->window = window;
    camera->position.x = position.x;
    camera->position.y = position.y;
    camera->angle = angle;
    camera->zoom = zoom;
    camera->meter = 100.0f;
    camera->width = width;
    camera->height = height;
    camera->_ui_begin_ = false;

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
    mm_free(*camera);
    *camera = NULL;
}


// Реализация API:


static void Camera2D_Impl_update(Camera2D *self) {
    if (!self) return;

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
    if (!self) return;

    self->width = width;
    self->height = height;
    self->window->renderer->viewport_resize(self->window->renderer, 0, 0, width, height);

    glm_mat4_identity(self->proj);
    float wdth = self->width/2.0f * self->meter/100.0f;
    float hght = self->height/2.0f * self->meter/100.0f;
    glm_ortho(-wdth, wdth, -hght, hght, -1.0, 1.0, self->proj);
    return;
}


static void Camera2D_Impl_ui_begin(Camera2D *self) {
    if (!self || self->_ui_begin_) return;
    self->_ui_begin_ = true;

    // Обнуляем матрицу вида в шейдере по умолчанию:
    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){-self->width/2, -self->height/2, 0});
    ShaderProgram *shader = self->window->renderer->default_shader;
    shader->begin(shader);
    shader->set_uniform_mat4(shader, "u_view", view);
    shader->end(shader);
}


static void Camera2D_Impl_ui_end(Camera2D *self) {
    if (!self || !self->_ui_begin_) return;
    self->_ui_begin_ = false;

    // Возвращаем обратно матрицу вида в шейдере по умолчанию:
    ShaderProgram *shader = self->window->renderer->default_shader;
    shader->begin(shader);
    shader->set_uniform_mat4(shader, "u_view", self->view);
    shader->end(shader);
}
