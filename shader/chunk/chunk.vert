#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;
in float elevation;
in vec2 intexcoord;
in vec3 normal;
in int tile;

uniform mat4 model, view, proj;
uniform vec3 lightdir;
uniform float logconst, zfar;

out vec2 texcoord;
out vec4 fcolor;
flat out int ftile;

void main()
{
	const float ambient=0.2;
	float col=max(dot(mat3(model)*normal,lightdir),0.0)+ambient;
    col*=(elevation-1.00)*600.0;
	vec4 r=proj*view*model*vec4(position,1.0);
	gl_Position=logDepth(r);
	texcoord=intexcoord;
	fcolor=vec4(vec3(col),1);
	ftile=tile;
}
