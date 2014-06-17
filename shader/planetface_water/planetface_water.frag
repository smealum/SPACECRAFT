#version 150 core

uniform sampler2DArray Texture;
uniform float alpha;

in float fluminosity;
flat in int ftile;
in vec2 ftexCoords;
in float frepeat;

out vec4 outColor;

void main()
{
    outColor = texture(Texture,vec3(ftexCoords.xy*frepeat,ftile));
	// XXX
	// outColor.xyz-=2.0*(1.0-outColor.a);
	outColor.xyz*=fluminosity;
	//outColor.a=alpha;
}
