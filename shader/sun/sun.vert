#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;

uniform float logconst = 10.0;
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);
uniform float znear = -1.0;
uniform float zfar = 1.0;

out vec3 vPos;

#define SunWidth (1.f)
//define SunWidth (108.f)

void main()
{	
	vPos = SunWidth * position;
	vec4 pos = proj * view * model * vec4(vPos, 1.0);
    gl_Position = logDepth(pos);
}
