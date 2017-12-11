uniform sampler2DArray Texture;

in vec2 texcoord;
in vec4 fcolor;
flat in int ftile;

out vec4 outColor;

void main()
{
    outColor = texture(Texture, vec3(texcoord.xy,ftile)) * fcolor;
	if(outColor.a < 0.01)discard;
}
