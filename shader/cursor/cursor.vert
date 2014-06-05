#version 150 core

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;

uniform mat4 model, view, proj;
uniform float logconst, zfar;

out vec4 fColor;

void main()
{
    fColor = vec4(1,0,0,1);
    gl_Position = proj * view * model * vec4(position, 1.0);
}
