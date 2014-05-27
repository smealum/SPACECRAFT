#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

uniform mat4 projection = mat4(1.0);
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform vec4 lightPosition = vec4(0.0,0.0,0.0,1.0);

smooth out vec4 fcolor;
smooth out vec3 fnormal;
smooth out vec4 fposition;
flat out vec3 flightPosition;


void main ()
{
    gl_Position = projection*view*model*vec4(position,1.0);
	fcolor=color;
	fposition=view*model*vec4(position,1.0);
    fnormal=(view*model*vec4(normal,0.0)).xyz;
	flightPosition=(view*lightPosition).xyz;
}
