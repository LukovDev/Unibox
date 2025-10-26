//
// test.c - Исходник, используемый для тестирования во время разработки.
//


// Подключаем:
#include <engine/engine.h>
#include <engine/core/graphics/gl.h>


GLuint VBO, VAO;
Camera2D *camera;
ShaderProgram *shader;


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
    self->set_fps(self, 10);
    self->set_vsync(self, false);

    camera = Camera2D_create(
        self, self->get_width(self), self->get_height(self),
        (Vec2d){0.0, 0.0}, 0.0f, 0.01f
    );

    // double start_time = Time_now(NULL);
    // DArray *arr = DArray_create(0);
    // for (int i=0; i<1024*1024*1024; i++)
    //     DArray_push(arr, (void*)camera);
    // DArray_clear(arr);
    // DArray_shrink(arr);
    // DArray_destroy(&arr);
    // printf("Time: %f\n", Time_now(NULL)-start_time);


    const char* vertex_shader_src = fs_load_file("data/shaders/default.vert", "r");
    const char* fragment_shader_src = fs_load_file("data/shaders/default.frag", "r");

    shader = ShaderProgram_create(self->renderer, vertex_shader_src, fragment_shader_src, NULL);
    shader->compile(shader);

    mm_free((char*)vertex_shader_src);
    mm_free((char*)fragment_shader_src);

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
        self->set_fps(self, 0);
    }
    if (self->get_is_defocused(self)) {
        printf("Defocused\n");
        self->set_fps(self, 10);
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
        printf("Mouse scroll: X=%d, Y=%d\n", input->get_mouse_wheel(self).x, input->get_mouse_wheel(self).y);
        printf("Mouse pos: X=%d, Y=%d\n", input->get_mouse_pos(self).x, input->get_mouse_pos(self).y);
        printf("Mouse visible: %s\n", input->get_mouse_visible(self) ? "true" : "false");
        printf("Key 1 pressed: %s\n", input->get_key_pressed(self)[K_UP] ? "true" : "false");
        printf("Key 1 down: %s\n", input->get_key_down(self)[K_UP] ? "true" : "false");
        printf("Key 1 up: %s\n", input->get_key_up(self)[K_UP] ? "true" : "false");
    }
    //input->set_mouse_pos(input, 100, 100);

    if (input->get_key_up(self)[K_f]) self->set_fullscreen(self, !self->get_fullscreen(self));
    if (input->get_key_up(self)[K_ESCAPE]) self->quit(self);

    if (input->get_key_up(self)[K_1]) self->maximize(self);
    if (input->get_key_up(self)[K_2]) self->minimize(self);
    if (input->get_key_up(self)[K_3]) self->restore(self);
    if (input->get_key_up(self)[K_4]) self->raise(self);
    if (input->get_key_up(self)[K_5]) self->set_always_top(self, !self->get_always_top(self));

    static int counter = 0;
    if (counter++ % 100 == 0) self->raise(self);

    self->set_title(self, "x: %g, y: %g", camera->position.x, camera->position.y);

    camera->zoom -= input->get_mouse_wheel(self).y * camera->zoom * 0.1f;
    if (input->get_key_pressed(self)[K_w]) camera->position.y += 10.0f * dtime;
    if (input->get_key_pressed(self)[K_a]) camera->position.x -= 10.0f * dtime;
    if (input->get_key_pressed(self)[K_s]) camera->position.y -= 10.0f * dtime;
    if (input->get_key_pressed(self)[K_d]) camera->position.x += 10.0f * dtime;
    if (input->get_mouse_pressed(self)[2]) {
        camera->position.x -= input->get_mouse_rel(self).x * camera->zoom;
        camera->position.y += input->get_mouse_rel(self).y * camera->zoom;
    }

    camera->update(camera);
}


// Вызывается каждый кадр (отрисовка окна):
void render(Window *self, Renderer *render, float dtime) {
    float t = self->get_time(self);
    float r = 0.5 + 0.5 * cosf(t + 0.0);
    float g = 0.5 + 0.5 * cosf(t + 2.0);
    float b = 0.5 + 0.5 * cosf(t + 4.0);
    render->clear(render, 0.0, 0.0, 0.0, 1.0f);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){r, g, 0});
    glm_rotate(model, glm_rad(g*360.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(r*360.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(b*360.0f), (vec3){0.0f, 0.0f, 1.0f});
    shader->begin(shader);
    shader->set_uniform_mat4(shader, "u_model", model);
    shader->set_uniform_float(shader, "u_time", self->get_time(self));
    shader->set_uniform_vec2(shader, "u_resolution", (Vec2f){self->get_width(self), self->get_height(self)});
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glm_mat4_identity(model);
    glm_translate(model, (vec3){b, -r, 0});
    glm_rotate(model, glm_rad(b*360.0f), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(g*360.0f), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(r*360.0f), (vec3){0.0f, 0.0f, 1.0f});
    shader->set_uniform_mat4(shader, "u_model", model);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->end(shader);

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
    ShaderProgram_destroy(&shader);
    Camera2D_destroy(&camera);
    printf("destroy done.\n");
}


// Точка входа в программу:
int main(int argc, char *argv[]) {
    printf("Engine version: %s\n", ENGINE_VERSION);

    Renderer *renderer = RendererGL_create(4, 1, true, RENDERER_GL_CORE);
    WinConfig *config = Window_create_config(start, update, render, resize, show, hide, destroy);
    Window *window = WindowSDL3_create(config, renderer);

    window->create(window);

    printf("(Before free) MM used: %g kb (%zu b). Blocks allocated: %zu. Absolute: %zu b. BlockHeaderSize: %zu b.\n",
            mm_get_used_size_kb(), mm_get_used_size(), mm_get_total_allocated_blocks(), mm_get_absolute_used_size(),
            mm_get_block_header_size());

    WindowSDL3_destroy(&window);
    Window_destroy_config(&config);
    RendererGL_destroy(&renderer);

    printf("(After free) MM used: %g kb (%zu b). Blocks allocated: %zu. Absolute: %zu b. BlockHeaderSize: %zu b.\n",
            mm_get_used_size_kb(), mm_get_used_size(), mm_get_total_allocated_blocks(), mm_get_absolute_used_size(),
            mm_get_block_header_size());
    if (mm_get_used_size() > 0) printf("Memory leak!\n");

    return 0;
}
