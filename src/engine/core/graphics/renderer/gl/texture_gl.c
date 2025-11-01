//
// texture_gl.c - Реализация работы с текстурами в OpenGL.
//


// Подключаем:
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../../mm/mm.h"
#include "../../gl.h"
#include "../../image.h"
#include "../../texture.h"
#include "texture_gl.h"


// Объявление функций:
static void TextureGL_Impl_begin(Texture *self);
static void TextureGL_Impl_end(Texture *self);
static void TextureGL_Impl_load(Texture *self, Image *image);
static void TextureGL_Impl_set_data(Texture *self, int width, int height, const void *data, bool use_mipmap,
                                    TextureFormat tex_format, TextureFormat data_format, TextureDataType data_type);
static Image* TextureGL_Impl_get_image(Texture *self, int channels);
static void TextureGL_Impl_set_filter(Texture *self, int name, int param);
static void TextureGL_Impl_set_linear(Texture *self);
static void TextureGL_Impl_set_pixelized(Texture *self);
static void TextureGL_Impl__destroy_(Texture *self);


// Регистрируем функции реализации апи для текстуры:
void TextureGL_RegisterAPI(Texture *texture) {
    texture->begin = TextureGL_Impl_begin;
    texture->end = TextureGL_Impl_end;
    texture->load = TextureGL_Impl_load;
    texture->set_data = TextureGL_Impl_set_data;
    texture->get_image = TextureGL_Impl_get_image;
    texture->set_filter = TextureGL_Impl_set_filter;
    texture->set_linear = TextureGL_Impl_set_linear;
    texture->set_pixelized = TextureGL_Impl_set_pixelized;
    texture->_destroy_ = TextureGL_Impl__destroy_;
}


// Реализация API:


static void TextureGL_Impl_begin(Texture *self) {
    if (!self) return;
    glBindTexture(GL_TEXTURE_2D, self->id);
    self->_is_begin_ = true;
}


static void TextureGL_Impl_end(Texture *self) {
    if (!self) return;
    glBindTexture(GL_TEXTURE_2D, 0);
    self->_is_begin_ = false;
}


static void TextureGL_Impl_load(Texture *self, Image *image) {
    if (!self) return;
    // Подбираем формат данных:
    TextureFormat tex_format;
    switch (image->channels) {
        case 1:  { tex_format = TEX_RED; break; }
        case 2:  { tex_format = TEX_RG; break; }
        case 3:  { tex_format = TEX_RGB; break; }
        case 4:  { tex_format = TEX_RGBA; break; }
        default: { tex_format = TEX_RGBA; break; }
    }
    // Выделяем память под данные:
    self->set_data(
        self, image->width, image->height, image->data, true,
        tex_format, tex_format, TEX_DATA_UBYTE
    );
}


static void TextureGL_Impl_set_data(
    Texture *self, int width, int height, const void *data, bool use_mipmap,
    TextureFormat tex_format, TextureFormat data_format, TextureDataType data_type) {
    if (!self) return;

    // Если размеры текущей текстуры не совпадает с передаваемыми данными, удаляем её:
    if (self->width != width || self->height != height) {
        self->_destroy_(self);
        self->width = width;
        self->height = height;
    }

    // Если текстура еще не создана, то создаем ее:
    if (!self->id) glGenTextures(1, &self->id);
    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, self->id);

    // Подбираем формат текстуры:
    int gl_tex_format;
    switch (tex_format) {
        case TEX_RGB:     { gl_tex_format = GL_RGB; break; }
        case TEX_RGBA:    { gl_tex_format = GL_RGBA; break; }
        case TEX_RGB16F:  { gl_tex_format = GL_RGB16F; break; }
        case TEX_RGBA16F: { gl_tex_format = GL_RGBA16F; break; }
        case TEX_RGB32F:  { gl_tex_format = GL_RGB32F; break; }
        case TEX_RGBA32F: { gl_tex_format = GL_RGBA32F; break; }
        case TEX_RED:     { gl_tex_format = GL_RED; break; }
        case TEX_R16F:    { gl_tex_format = GL_R16F; break; }
        case TEX_SRGB:    { gl_tex_format = GL_SRGB8; break; }
        case TEX_SRGBA:   { gl_tex_format = GL_SRGB8_ALPHA8; break; }
        default:          { gl_tex_format = GL_RGBA; break; }
    }

    // Подбираем формат внешних данных:
    int gl_data_format;
    switch (data_format) {
        case TEX_RED:  { gl_data_format = GL_RED; break; }
        case TEX_RG:   { gl_data_format = GL_RG; break; }
        case TEX_RGB:  { gl_data_format = GL_RGB; break; }
        case TEX_RGBA: { gl_data_format = GL_RGBA; break; }
        case TEX_BGR:  { gl_data_format = GL_BGR; break; }
        case TEX_BGRA: { gl_data_format = GL_BGRA; break; }
        default:       { gl_data_format = GL_RGBA; break; }
    }

    // Подбираем тип данных:
    int gl_data_type;
    switch (data_type) {
        case TEX_DATA_UBYTE:  { gl_data_type = GL_UNSIGNED_BYTE; break; }
        case TEX_DATA_BYTE:   { gl_data_type = GL_BYTE; break; }
        case TEX_DATA_USHORT: { gl_data_type = GL_UNSIGNED_SHORT; break; }
        case TEX_DATA_SHORT:  { gl_data_type = GL_SHORT; break; }
        case TEX_DATA_UINT:   { gl_data_type = GL_UNSIGNED_INT; break; }
        case TEX_DATA_INT:    { gl_data_type = GL_INT; break; }
        case TEX_DATA_FLOAT:  { gl_data_type = GL_FLOAT; break; }
        default:              { gl_data_type = GL_UNSIGNED_BYTE; break; }
    }

    // Загрузка данных текстуры:
    glTexImage2D(GL_TEXTURE_2D, 0, gl_tex_format, width, height, 0, gl_data_format, gl_data_type, data);

    // Если надо использовать мипмапы, создаём их:
    if (use_mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, 0);
}


static Image* TextureGL_Impl_get_image(Texture *self, int channels) {
    if (!self) return NULL;

    // Выделяем память под данные:
    unsigned char* data = mm_alloc(self->width * self->height * channels);
    if (!data) mm_alloc_error();

    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, self->id);
    // Подбираем формат данных:
    int gl_data_format;
    switch (channels) {
        case 1:  { gl_data_format = GL_RED; break; }
        case 2:  { gl_data_format = GL_RG; break; }
        case 3:  { gl_data_format = GL_RGB; break; }
        case 4:  { gl_data_format = GL_RGBA; break; }
        default: { gl_data_format = GL_RGBA; break; }
    }
    glGetTexImage(GL_TEXTURE_2D, 0, gl_data_format, GL_UNSIGNED_BYTE, data);
    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, 0);

    // Создаём изображение:
    Image* img = mm_alloc(sizeof(Image));
    if (!img) mm_alloc_error();

    img->width = self->width;
    img->height = self->height;
    img->channels = channels;
    img->from_stbi = false;
    img->data = data;

    return img;
}


static void TextureGL_Impl_set_filter(Texture *self, int name, int param) {
    if (!self) return;
    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, self->id);
    glTexParameteri(GL_TEXTURE_2D, name, param);
    if (!self->_is_begin_) glBindTexture(GL_TEXTURE_2D, 0);
}


static void TextureGL_Impl_set_linear(Texture *self) {
    if (!self) return;
    self->set_filter(self, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    self->set_filter(self, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


static void TextureGL_Impl_set_pixelized(Texture *self) {
    if (!self) return;
    self->set_filter(self, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    self->set_filter(self, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}


static void TextureGL_Impl__destroy_(Texture *self) {
    if (!self) return;
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &self->id);
    self->_is_begin_ = false;
    self->id = 0;
}
