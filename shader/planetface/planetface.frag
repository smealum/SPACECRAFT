uniform sampler2DArray Texture;
uniform float alpha;

in float fluminosity;
flat in int ftile;
in vec2 ftexCoords;
flat in float frepeat;

out vec4 outColor;

void main() {
  outColor = texture(Texture,vec3(ftexCoords.xy*frepeat,ftile));
	outColor.xyz *= fluminosity;
	outColor.a = 1.0;
}
