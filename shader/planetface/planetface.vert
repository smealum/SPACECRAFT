#version 150 core

in vec3 position;
in float elevation;

uniform mat4 model, view, proj;
uniform vec3 v1, v2;
uniform vec4 overrideColor;

out vec3 pos;
out vec4 Color;
out vec3 gv1, gv2;
out float gelevation;

void main()
{
	// vec3 n=normalize(position);
    //Color = vec4(vec3((elevation-1.0f)*10),1.0);
    Color=vec4(abs(v1),1.0);
    // gl_Position = proj * view * model * vec4(position, 1.0);
    pos=position;
    gv1=v1;
    gv2=v2;
    gelevation=elevation;
}
