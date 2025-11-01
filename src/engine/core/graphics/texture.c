//
// texture.c - Создаёт код для работы с текстурами.
//


// Подключаем:
#include <stdio.h>
#include "../mm/mm.h"
#include "realization.h"
#include "texture.h"


// Создать текстуру:
Texture* Texture_create(Renderer *renderer) {
    if (!renderer) return NULL;

    Texture *texture = mm_calloc(1, sizeof(Texture));
    if (!texture) mm_alloc_error();

    // Заполняем поля:
    texture->renderer = renderer;
    texture->id = 0;
    texture->_is_begin_ = false;
    texture->width = 1;
    texture->height = 1;
    texture->channels = 4;

    // Регистрируем функции для определенного рендерера:
    switch (renderer->type) {
        case RENDERER_OPENGL:
            TextureGL_RegisterAPI(texture);
            break;

        // Other renderers.

        default: {
            const char* err = "Unknown renderer type.";
            fprintf(stderr, "Texture_create: %s\n", err);
            return NULL;
        }
    }
    return texture;
}


// Уничтожить текстуру:
void Texture_destroy(Texture **texture) {
    if (!texture || !*texture) return;

    // Удаляем саму текстуру:
    (*texture)->_destroy_(*texture);

    // Освобождаем структуру:
    mm_free(*texture);
    *texture = NULL;
}
