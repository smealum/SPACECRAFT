#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;

uniform mat4 projection = mat4(1.0);
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);

smooth out vec4 fcolor;
smooth out vec3 fnormal;
smooth out vec3 flightDir;
smooth out vec3 feyeDir;

uniform vec4 lightPosition = vec4(1.0,0.0,0.0,1.0);

void main ()
{
    fnormal=(view*model*vec4(normal,0.0)).xyz;

	fcolor = color;

    vec4 vVertex = view*model*vec4(position,1.0);
    gl_Position = projection*vVertex;
    feyeDir = -vVertex.xyz;

    flightDir = lightPosition.xyz - vVertex.xyz;
}
