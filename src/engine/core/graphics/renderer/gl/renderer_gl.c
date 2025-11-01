//
// renderer_gl.c - Реализует обертку над OpenGL на основе абстрактного апи.
//


// Подключаем:
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../../math.h"
#include "../../../mm/mm.h"
#include "../../renderer.h"
#include "../../gl.h"
#include "../../camera.h"
#include "../../shader.h"
#include "renderer_gl.h"


// Стандартные шейдеры рендеринга:
static const char* DEFAULT_SHD_VERT = \
"#version 330 core\n"
"uniform mat4 u_model = mat4(1.0);\n"
"uniform mat4 u_view = mat4(1.0);\n"
"uniform mat4 u_proj = mat4(1.0);\n"
"layout (location = 0) in vec3 a_position;\n"
"layout (location = 1) in vec2 a_texcoord;\n"
"out vec2 TexCoord;\n"
"void main(void) {\n"
"    gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);\n"
"    TexCoord = a_texcoord;\n"
"}\n";

static const char* DEFAULT_SHD_FRAG = \
"#version 330 core\n"
"uniform bool u_use_points = false;\n"
"uniform bool u_use_texture;\n"
"uniform vec4 u_color = vec4(1.0);\n"
"uniform sampler2D u_texture;\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"void main(void) {\n"
"    // Если мы используем точки для рисования:\n"
"    if (u_use_points) {\n"
"        vec2 coord = gl_PointCoord*2.0-1.0;\n"
"        if (dot(coord, coord) > 1.0) discard;  // Отбрасываем всё за пределами круга.\n"
"    }\n"
"    // Если мы используем текстуру, рисуем с ней, иначе только цвет:\n"
"    if (u_use_texture) {\n"
"        FragColor = u_color * texture(u_texture, TexCoord);\n"
"    } else {\n"
"        FragColor = u_color;\n"
"    }\n"
"}\n";


// Объявление функций:
static void RendererGL_Impl_init(Renderer *self);
static void RendererGL_Impl_clear(Renderer *self, float r, float g, float b, float a);
static void RendererGL_Impl_camera2d_update(Renderer *self);
static void RendererGL_Impl_camera3d_update(Renderer *self);
static void RendererGL_Impl_viewport_resize(Renderer *self, int width, int height);


// Регистрируем функции реализации апи:
static void RendererGL_RegisterAPI(Renderer *self) {
    self->init = RendererGL_Impl_init;
    self->clear = RendererGL_Impl_clear;
    self->camera2d_update = RendererGL_Impl_camera2d_update;
    self->camera3d_update = RendererGL_Impl_camera3d_update;
    self->viewport_resize = RendererGL_Impl_viewport_resize;
}


// Создать рендерер:
Renderer* RendererGL_create(int major, int minor, bool doublebuffer, RendererGL_Profile profile) {
    Renderer *renderer = (Renderer*)mm_alloc(sizeof(Renderer));
    if (!renderer) mm_alloc_error();

    // Создаём данные рендерера:
    RendererGL_Data *data = (RendererGL_Data*)mm_calloc(1, sizeof(RendererGL_Data));
    if (!data) mm_alloc_error();

    // Заполняем поля данных:
    data->major = major;
    data->minor = minor;
    data->doublebuffer = doublebuffer;
    data->profile = profile;

    // Заполняем поля рендерера:
    renderer->name = "OpenGL";
    renderer->type = RENDERER_OPENGL;
    renderer->default_shader = NULL;
    renderer->camera = NULL;
    renderer->data = data;

    // Создаём шейдер:
    ShaderProgram *default_shader = ShaderProgram_create(renderer, DEFAULT_SHD_VERT, DEFAULT_SHD_FRAG, NULL);
    if (!default_shader || default_shader->get_error(default_shader)) {
        fprintf(stderr, "RENDERER_GL-FAIL: Creating default shader failed: %s\n", default_shader->error);
        // Самоуничтожение при провале создания шейдера:
        ShaderProgram_destroy(&default_shader);
        mm_free(data);
        mm_free(renderer);
        return NULL;
    }
    renderer->default_shader = default_shader;

    // Регистрируем функции:
    RendererGL_RegisterAPI(renderer);

    return renderer;
}


// Уничтожить рендерер:
void RendererGL_destroy(Renderer **self) {
    if (!self || !*self) return;

    // Освобождаем память данных рендерера:
    if ((*self)->data) {
        mm_free((*self)->data);
        (*self)->data = NULL;
    }

    // Освобождаем память шейдера:
    if ((*self)->default_shader) {
        ShaderProgram_destroy(&(*self)->default_shader);
    }

    // Освободить память рендерера:
    mm_free(*self);
    *self = NULL;
}


// Реализация API:


static void RendererGL_Impl_init(Renderer *self) {
    if (!gladLoadGL()) {
        fprintf(stderr, "RENDERER_GL-FAIL: gladLoadGL failed.\n");
        exit(1);
        return;
    }

    glEnable(GL_BLEND);  // Включаем смешивание цветов.

    // Устанавливаем режим смешивания:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Разрешаем установку размера точки через шейдер:
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Делаем нулевой текстурный юнит привязанным к нулевой текстуре:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Компилируем дефолтный шейдер:
    self->default_shader->compile(self->default_shader);
}


static void RendererGL_Impl_clear(Renderer *self, float r, float g, float b, float a) {
    if (!self) return;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


static void RendererGL_Impl_camera2d_update(Renderer *self) {
    glDisable(GL_DEPTH_TEST);
    ShaderProgram *shader = self->default_shader;
    Camera2D *camera = (Camera2D*)self->camera;
    if (!shader || !camera) return;
    shader->begin(shader);
    shader->set_uniform_mat4(shader, "u_view", camera->view);
    shader->set_uniform_mat4(shader, "u_proj", camera->proj);
}


static void RendererGL_Impl_camera3d_update(Renderer *self) {
    // ...
}


static void RendererGL_Impl_viewport_resize(Renderer *self, int width, int height) {
    glViewport(0, 0, width, height);
}
