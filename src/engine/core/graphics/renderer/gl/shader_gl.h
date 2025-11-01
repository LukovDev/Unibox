//
// shader_gl.h
//

#pragma once


// Объявление структур:
typedef struct ShaderProgram ShaderProgram;


// Регистрируем функции реализации апи для шейдера:
void ShaderGL_RegisterAPI(ShaderProgram *shader);
