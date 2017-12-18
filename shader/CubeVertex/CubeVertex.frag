uniform sampler2DArray Texture;

in vec2 f_texture_coordinate;
in float luminancy;
flat in int f_tile;

out vec4 outColor;

void main() {
  outColor = texture(Texture,vec3(f_texture_coordinate.xy,f_tile));
  outColor.rgb *= luminancy;
  if (outColor.a < 0.01) discard;
}
