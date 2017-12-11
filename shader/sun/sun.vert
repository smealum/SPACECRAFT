#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;

uniform float logconst;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float znear;
uniform float zfar;

out vec3 vPos;

#define SunWidth (1.f)
//#define SunWidth (108.f)

void main()
{	
	vPos = SunWidth * position;
	vec4 pos = proj * view * model * vec4(vPos, 1.0);
    gl_Position = logDepth(pos);
}
