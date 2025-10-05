//
// test.c - Исходник, используемый для тестирования во время разработки.
//


// Подключаем:
#include <stdio.h>
#include <stdlib.h>
#include <engine/engine.h>
#include <engine/core/graphics/gl.h>
#include <cglm/cglm.h>
#include <math.h>


GLuint VBO, VAO;
GLuint shader_program;
Camera2D *camera;


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
    self->set_fps(self, 0);
    self->set_vsync(self, false);

    camera = Camera2D_create(
        self, self->get_width(self), self->get_height(self),
        (Vec2d){0.0, 0.0}, 0.0f, 0.01f
    );

    const char* vertex_shader_src = fs_load_file("data/shaders/default.vert", "r");
    const char* fragment_shader_src = fs_load_file("data/shaders/default.frag", "r");

    // Вершины треугольника
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment);

    mm_free((char*)vertex_shader_src);
    mm_free((char*)fragment_shader_src);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    glLinkProgram(shader_program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // === Вершины треугольника ===
    float vertices[] = {
         0.0f,  0.5f, 0.0f,  // верх
        -0.5f, -0.5f, 0.0f,  // левый
         0.5f, -0.5f, 0.0f   // правый
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Настройка VAO/VBO
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // layout(location = 0) → 3 float'а
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


// Вызывается каждый кадр (цикл окна):
void update(Window *self, Input *input, float dtime) {
    printf("Update called. FPS %f\n", self->get_current_fps(self));
    if (self->get_is_focused(self)) {
        printf("Focused\n");
    }
    if (self->get_is_defocused(self)) {
        printf("Defocused\n");
    }

    if (false) {
        system("cls");
        printf("Mouse pressed: 0=%s, 1=%s, 2=%s, 3=%s, 4=%s\n",
            input->get_mouse_pressed(self)[0] ? "true" : "false",
            input->get_mouse_pressed(self)[1] ? "true" : "false",
            input->get_mouse_pressed(self)[2] ? "true" : "false",
            input->get_mouse_pressed(self)[3] ? "true" : "false",
            input->get_mouse_pressed(self)[4] ? "true" : "false"
        );
        printf("Mouse down: 0=%s, 1=%s, 2=%s, 3=%s, 4=%s\n",
            input->get_mouse_down(self)[0] ? "true" : "false",
            input->get_mouse_down(self)[1] ? "true" : "false",
            input->get_mouse_down(self)[2] ? "true" : "false",
            input->get_mouse_down(self)[3] ? "true" : "false",
            input->get_mouse_down(self)[4] ? "true" : "false"
        );
        printf("Mouse up: 0=%s, 1=%s, 2=%s, 3=%s, 4=%s\n",
            input->get_mouse_up(self)[0] ? "true" : "false",
            input->get_mouse_up(self)[1] ? "true" : "false",
            input->get_mouse_up(self)[2] ? "true" : "false",
            input->get_mouse_up(self)[3] ? "true" : "false",
            input->get_mouse_up(self)[4] ? "true" : "false"
        );
        printf("Mouse rel: X=%d, Y=%d\n", input->get_mouse_rel(self).x, input->get_mouse_rel(self).y);
        printf("Mouse focused: %s\n", input->get_mouse_focused(self) ? "true" : "false");
        printf("Mouse scroll: X=%d, Y=%d\n", input->get_mouse_scroll(self).x, input->get_mouse_scroll(self).y);
        printf("Mouse pos: X=%d, Y=%d\n", input->get_mouse_pos(self).x, input->get_mouse_pos(self).y);
        printf("Mouse visible: %s\n", input->get_mouse_visible(self) ? "true" : "false");
        printf("Key 1 pressed: %s\n", input->get_key_pressed(self)[K_UP] ? "true" : "false");
        printf("Key 1 down: %s\n", input->get_key_down(self)[K_UP] ? "true" : "false");
        printf("Key 1 up: %s\n", input->get_key_up(self)[K_UP] ? "true" : "false");
    }
    //input->set_mouse_pos(input, 100, 100);

    if (input->get_key_up(self)[K_f]) self->set_fullscreen(self, !self->get_fullscreen(self));
    if (input->get_key_up(self)[K_ESCAPE]) self->quit(self);

    self->set_title(self, "x: %g, y: %g", camera->position.x, camera->position.y);

    if (input->get_key_pressed(self)[K_w]) camera->position.y += 10.0f * dtime;
    if (input->get_key_pressed(self)[K_a]) camera->position.x -= 10.0f * dtime;
    if (input->get_key_pressed(self)[K_s]) camera->position.y -= 10.0f * dtime;
    if (input->get_key_pressed(self)[K_d]) camera->position.x += 10.0f * dtime;
    camera->update(camera);
}


// Вызывается каждый кадр (отрисовка окна):
void render(Window *self, Renderer *render, float dtime) {
    float t = self->get_time(self);
    float r = 0.5 + 0.5 * cosf(t + 0.0);
    float g = 0.5 + 0.5 * cosf(t + 2.0);
    float b = 0.5 + 0.5 * cosf(t + 4.0);
    render->clear(render, r, g, b, 1.0f);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    self->display(self);
}


// Вызывается при изменении размера окна:
void resize(Window *self, int width, int height) {
    printf("ReSize called. New W: %d, H: %d\n", width, height);
    camera->resize(camera, width, height);
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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    Camera2D_destroy(&camera);
}


// Точка входа в программу:
int main(int argc, char *argv[]) {
    printf("Engine version: %s\n", ENGINE_VERSION);

    Renderer *renderer = RendererGL_create(4, 1, true, RENDERER_GL_CORE);
    WinConfig *config = Window_create_config(start, update, render, resize, show, hide, destroy);
    Window *window = WindowSDL3_create(config, renderer);
    config->fps = 10.0;

    window->create(window);

    printf("(Before free) Memory used: %g kb (%zu b).\n", mm_get_used_size_kb(), mm_get_used_size());

    WindowSDL3_destroy(&window);
    Window_destroy_config(&config);
    RendererGL_destroy(&renderer);

    printf("(After free) Memory used: %g kb (%zu b).\n", mm_get_used_size_kb(), mm_get_used_size());
    if (mm_get_used_size() > 0) printf("Memory leak!\n");

    return 0;
}
