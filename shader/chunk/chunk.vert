#version 150 core

in vec3 position;
in vec2 texcoord;
in float facedir;

uniform mat4 model, view, proj;

out vec3 pos;
out vec2 gtexcoord;
out vec4 gcolor;
out int dir;

void main()
{
	gcolor = vec4(0.0,1.0,0.0,1.0);
    pos = position;
    gtexcoord = texcoord;
    dir = int(facedir);
}
