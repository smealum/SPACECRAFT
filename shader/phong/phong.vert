#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

uniform mat4 projection = mat4(1.0);
uniform mat4 modelview = mat4(1.0);

smooth out vec4 fcolor;
smooth out vec3 fnormal;

smooth out vec4 fposition;
flat out vec3 flightPosition;

uniform vec4 lightPosition = vec4(1.0,1.0,1.0,0.0);

void main ()
{
    gl_Position = projection*modelview*vec4(position,1.0);
	fcolor=color;
	fposition=modelview*vec4(position,1.0);
    fnormal=(modelview*vec4(normal,0.0)).xyz;
	flightPosition=(modelview*lightPosition).xyz;
}
