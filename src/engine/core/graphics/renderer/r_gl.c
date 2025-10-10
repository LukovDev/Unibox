//
// r_gl.c - Реализует обертку над OpenGL на основе абстрактного апи.
//


// Подключаем:
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../math.h"
#include "../../mm/mm.h"
#include "../renderer.h"
#include "../gl.h"
#include "../camera.h"
#include "../shader.h"
#include "r_gl.h"


// Объявление функций:
static void RendererGL_Impl_init(Renderer *self);
static void RendererGL_Impl_clear(Renderer *self, float r, float g, float b, float a);
static void RendererGL_Impl_camera2d_update(Renderer *self);
static void RendererGL_Impl_camera3d_update(Renderer *self);
static void RendererGL_Impl_viewport_resize(Renderer *self, int width, int height);

static void ShaderGL_Impl_compile(ShaderProgram *self);
static void ShaderGL_Impl_begin(ShaderProgram *self);
static void ShaderGL_Impl_end(ShaderProgram *self);
static uint32_t ShaderGL_Impl_get_location(ShaderProgram *self, const char* name);
static void ShaderGL_Impl_set_uniform_bool(ShaderProgram *self, const char* name, bool value);
static void ShaderGL_Impl_set_uniform_int(ShaderProgram *self, const char* name, int value);
static void ShaderGL_Impl_set_uniform_float(ShaderProgram *self, const char* name, float value);
static void ShaderGL_Impl_set_uniform_vec2(ShaderProgram *self, const char* name, Vec2f value);
static void ShaderGL_Impl_set_uniform_vec3(ShaderProgram *self, const char* name, Vec3f value);
static void ShaderGL_Impl_set_uniform_vec4(ShaderProgram *self, const char* name, Vec4f value);
static void ShaderGL_Impl_set_uniform_mat2(ShaderProgram *self, const char* name, mat2 value);
static void ShaderGL_Impl_set_uniform_mat3(ShaderProgram *self, const char* name, mat3 value);
static void ShaderGL_Impl_set_uniform_mat4(ShaderProgram *self, const char* name, mat4 value);
static void ShaderGL_Impl_set_uniform_mat2x3(ShaderProgram *self, const char* name, mat2x3 value);
static void ShaderGL_Impl_set_uniform_mat3x2(ShaderProgram *self, const char* name, mat3x2 value);
static void ShaderGL_Impl_set_uniform_mat2x4(ShaderProgram *self, const char* name, mat2x4 value);
static void ShaderGL_Impl_set_uniform_mat4x2(ShaderProgram *self, const char* name, mat4x2 value);
static void ShaderGL_Impl_set_uniform_mat3x4(ShaderProgram *self, const char* name, mat3x4 value);
static void ShaderGL_Impl_set_uniform_mat4x3(ShaderProgram *self, const char* name, mat4x3 value);


// Регистрируем функции реализации апи:
static void RendererGL_RegisterAPI(Renderer *self) {
    self->init = RendererGL_Impl_init;
    self->clear = RendererGL_Impl_clear;
    self->camera2d_update = RendererGL_Impl_camera2d_update;
    self->camera3d_update = RendererGL_Impl_camera3d_update;
    self->viewport_resize = RendererGL_Impl_viewport_resize;
}


// Регистрируем функции реализации апи для шейдера:
void RendererGL_RegisterShaderProgramAPI(ShaderProgram *shader) {
    shader->compile = ShaderGL_Impl_compile;
    shader->begin = ShaderGL_Impl_begin;
    shader->end = ShaderGL_Impl_end;
    shader->get_location = ShaderGL_Impl_get_location;
    shader->set_uniform_bool = ShaderGL_Impl_set_uniform_bool;
    shader->set_uniform_int = ShaderGL_Impl_set_uniform_int;
    shader->set_uniform_float = ShaderGL_Impl_set_uniform_float;
    shader->set_uniform_vec2 = ShaderGL_Impl_set_uniform_vec2;
    shader->set_uniform_vec3 = ShaderGL_Impl_set_uniform_vec3;
    shader->set_uniform_vec4 = ShaderGL_Impl_set_uniform_vec4;
    shader->set_uniform_mat2 = ShaderGL_Impl_set_uniform_mat2;
    shader->set_uniform_mat3 = ShaderGL_Impl_set_uniform_mat3;
    shader->set_uniform_mat4 = ShaderGL_Impl_set_uniform_mat4;
    shader->set_uniform_mat2x3 = ShaderGL_Impl_set_uniform_mat2x3;
    shader->set_uniform_mat3x2 = ShaderGL_Impl_set_uniform_mat3x2;
    shader->set_uniform_mat2x4 = ShaderGL_Impl_set_uniform_mat2x4;
    shader->set_uniform_mat4x2 = ShaderGL_Impl_set_uniform_mat4x2;
    shader->set_uniform_mat3x4 = ShaderGL_Impl_set_uniform_mat3x4;
    shader->set_uniform_mat4x3 = ShaderGL_Impl_set_uniform_mat4x3;
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
    renderer->camera = NULL;
    renderer->data = data;

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

    // Освободить память рендерера:
    mm_free(*self);
    *self = NULL;
}


// Реализация API для шейдера:


static uint32_t compile_shader(ShaderProgram *program, const char* source, GLenum type) {
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* log = mm_alloc(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, log);
        // Тип шейдера:
        const char* type_str = (type == GL_VERTEX_SHADER)   ? "VERTEX"   :
                               (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :
                               (type == GL_GEOMETRY_SHADER) ? "GEOMETRY" : "UNKNOWN";
        // Сколько надо выделить памяти:
        int needed = snprintf(NULL, 0, "ShaderCompileError (%s):\n%s\n", type_str, log);
        program->error = mm_alloc(needed + 1);
        // Форматируем строку:
        sprintf(program->error, "ShaderCompileError (%s):\n%s\n", type_str, log);
        fprintf(stderr, "%s", program->error);
        mm_free(log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


static void ShaderGL_Impl_compile(ShaderProgram *self) {
    if (!self) return;

    uint32_t program = glCreateProgram();
    uint32_t shaders[3] = {0};

    // Компилируем шейдеры:
    if (self->vertex)   shaders[0] = compile_shader(self, self->vertex, GL_VERTEX_SHADER);
    if (self->fragment) shaders[1] = compile_shader(self, self->fragment, GL_FRAGMENT_SHADER);
    if (self->geometry) shaders[2] = compile_shader(self, self->geometry, GL_GEOMETRY_SHADER);

    // Линкуем программу:
    for (int i = 0; i < 3; ++i) {
        if (shaders[i]) glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    // Проверяем статус линковки:
    int linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        char* log = mm_alloc(logLength);
        glGetProgramInfoLog(program, logLength, NULL, log);
        // Сколько надо выделить памяти:
        int needed = snprintf(NULL, 0, "ShaderLinkingError:\n%s\n", log);
        self->error = mm_alloc(needed + 1);
        // Форматируем строку:
        sprintf(self->error, "ShaderCompileError:\n%s\n", log);
        fprintf(stderr, "%s", self->error);
        mm_free(log);
        glDeleteProgram(program);
        return;
    }

    // Удаляем отдельные шейдеры:
    for (int i = 0; i < 3; ++i) {
        if (shaders[i]) {
            glDetachShader(program, shaders[i]);
            glDeleteShader(shaders[i]);
        }
    }
    self->id = program;
}


static void ShaderGL_Impl_begin(ShaderProgram *self) {
    if (!self) return;
    glGetIntegerv(GL_CURRENT_PROGRAM, &self->_id_before_begin_);
    glUseProgram(self->id);
    self->_is_begin_ = true;
}


static void ShaderGL_Impl_end(ShaderProgram *self) {
    if (!self) return;
    glUseProgram((uint32_t)self->_id_before_begin_);
    self->_is_begin_ = false;
}


static uint32_t ShaderGL_Impl_get_location(ShaderProgram *self, const char* name) {
    if (!self) return -1;
    // TODO: Caching...
    int location = glGetUniformLocation(self->id, name);
    return location;
}


static void ShaderGL_Impl_set_uniform_bool(ShaderProgram *self, const char* name, bool value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform1i(loc, (int)value);
}


static void ShaderGL_Impl_set_uniform_int(ShaderProgram *self, const char* name, int value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform1i(loc, value);
}


static void ShaderGL_Impl_set_uniform_float(ShaderProgram *self, const char* name, float value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform1f(loc, value);
}


static void ShaderGL_Impl_set_uniform_vec2(ShaderProgram *self, const char* name, Vec2f value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform2fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_vec3(ShaderProgram *self, const char* name, Vec3f value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform3fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_vec4(ShaderProgram *self, const char* name, Vec4f value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniform4fv(loc, 1, (float*)&value);
}


static void ShaderGL_Impl_set_uniform_mat2(ShaderProgram *self, const char* name, mat2 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3(ShaderProgram *self, const char* name, mat3 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4(ShaderProgram *self, const char* name, mat4 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat2x3(ShaderProgram *self, const char* name, mat2x3 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix2x3fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3x2(ShaderProgram *self, const char* name, mat3x2 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix3x2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat2x4(ShaderProgram *self, const char* name, mat2x4 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix2x4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4x2(ShaderProgram *self, const char* name, mat4x2 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix4x2fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat3x4(ShaderProgram *self, const char* name, mat3x4 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix3x4fv(loc, 1, GL_FALSE, (float*)value);
}


static void ShaderGL_Impl_set_uniform_mat4x3(ShaderProgram *self, const char* name, mat4x3 value) {
    if (!self) return;
    uint32_t loc = self->get_location(self, name);
    glUniformMatrix4x3fv(loc, 1, GL_FALSE, (float*)value);
}


// Реализация API:


static void RendererGL_Impl_init(Renderer *self) {
    if (!gladLoadGL()) {
        fprintf(stderr, "R_GL-FAIL: gladLoadGL failed.\n");
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
}


static void RendererGL_Impl_clear(Renderer *self, float r, float g, float b, float a) {
    if (!self) return;
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


static void RendererGL_Impl_camera2d_update(Renderer *self) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(1);
    glUniformMatrix4fv(glGetUniformLocation(1, "u_view"), 1, GL_FALSE, (float*)((Camera2D*)self->camera)->view);
    glUniformMatrix4fv(glGetUniformLocation(1, "u_projection"), 1, GL_FALSE, (float*)((Camera2D*)self->camera)->proj);
}


static void RendererGL_Impl_camera3d_update(Renderer *self) {
    // ...
}


static void RendererGL_Impl_viewport_resize(Renderer *self, int width, int height) {
    glViewport(0, 0, width, height);
}
