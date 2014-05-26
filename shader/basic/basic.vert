#version 150 core

in vec3 position;
in vec4 color;
in vec2 texcoord;
uniform mat4 model, view, proj;
uniform vec4 overrideColor;

out vec4 Color;
out vec2 Texcoord;

void main()
{
    Color = overrideColor * color;
    Texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
