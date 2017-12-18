#define logDepthZ(v) ((log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w)
#define logDepth(v) vec4(v.xy,logDepthZ(v),v.w)

// CubeVertex {
     in vec3 position;
     in vec3 normal;
     in vec2 texture_coordinate;
     in float tile;
// };

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float logconst;
uniform vec3 lightdir;

out vec2 f_texture_coordinate;
out float luminancy;
flat out int f_tile;

void main() {
  vec4 f_position = proj * view * model * vec4(position, 1.0);
  gl_Position = logDepth(f_position);

  vec3 f_normal = normalize(vec3(model * vec4(normal, 0.0)));
  luminancy = 0.2 + max(dot(lightdir, f_normal), 0.0);

  f_texture_coordinate = texture_coordinate;
  f_tile = int(tile);
}
