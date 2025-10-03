#version 330 core

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_projection = mat4(1.0);

layout (location = 0) in vec3 a_position;
out vec3 vertexColor;

void main() {
   gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
   vertexColor = a_position + 0.5;
}
