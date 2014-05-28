#version 150 core

in vec3 position;
in vec3 color;
uniform mat4 model, view, proj;
uniform vec4 overrideColor;

out vec4 Color;

void main()
{
    Color = overrideColor;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
