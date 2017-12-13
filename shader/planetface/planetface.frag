uniform sampler2DArray Texture;
uniform float alpha;

in vec2 f_texture_coordinate;
in float luminancy;
flat in int f_tile;

out vec4 outColor;

void main() {
  outColor = texture(Texture,vec3(f_texture_coordinate.xy,f_tile));
  outColor.rgb *= luminancy;
	outColor.a = 1.0;
}
