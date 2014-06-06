#version 150 core

uniform sampler2DArray Texture;

in vec2 texcoord;
in vec4 fcolor;
in flat int ftile;

out vec4 outColor;

void main()
{
    // outColor = fcolor;
    outColor = texture(Texture, vec3(texcoord.xy,ftile)) * fcolor;
	if (outColor.a < 0.01)
		discard;
}
