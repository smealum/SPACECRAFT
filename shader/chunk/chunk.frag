#version 150 core

uniform sampler2DArray Texture;

in vec2 texcoord;
in vec4 fcolor;

out vec4 outColor;

void main()
{
    // outColor = fcolor;
    outColor = texture(Texture, vec3(texcoord.xy,0)) * fcolor;
	if (outColor.a < 0.01)
		discard;
}
