#version 150 core

uniform sampler2D Texture;

in vec2 texcoord;
in vec4 fcolor;

out vec4 outColor;

void main()
{
    // outColor = fcolor;
    outColor = texture(Texture, texcoord)*fcolor;
	if (outColor.a < 0.01)
		discard;
}
