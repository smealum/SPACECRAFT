#version 150 core

uniform sampler2DArray Texture;

in float fluminosity;
in flat int ftile;
in vec2 ftexCoords;
in float frepeat;

out vec4 outColor;

void main()
{
    outColor = texture(Texture,vec3(ftexCoords.xy*frepeat,ftile));
    outColor.xyz *= fluminosity;
	outColor.a = 1.0;
	// XXX
	//outColor.xyz-=2.0*(1.0-outColor.a);
}
