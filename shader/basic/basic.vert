in vec3 position;
in vec4 color;
in vec2 texcoord;
uniform mat4 model, view, proj;
uniform vec4 overrideColor;
uniform vec4 tmp[4];

out vec4 Color;
out vec2 Texcoord;

void main()
{
    Color = overrideColor * color * vec4(tmp[0].r, tmp[1].g, tmp[2].b, 1);
    Texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
