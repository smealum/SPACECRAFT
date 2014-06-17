#version 330 core

uniform sampler2DArray Texture;

flat in int ftile;
in vec2 ftexCoords;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, vec3(ftexCoords.xy, ftile));
	/*outColor = vec4(1, 0, 0, 1);*/
}
