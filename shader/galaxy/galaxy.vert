#version 330

// #define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

layout(location = 0) in vec3 position;

uniform mat4 proj = mat4(1.0);
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);

uniform float logconst, zfar;

void main ()
{
    vec4 p = proj*view*model*vec4(position,1.0);
	// gl_Position = logDepth(p);
	gl_Position = vec4(p.xy,0.9*p.w,p.w);
}
