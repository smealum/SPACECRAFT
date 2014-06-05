#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;

uniform float logconst = 10.0;
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);
uniform float znear = -1.0;
uniform float zfar = 1.0;

out vec3 e2p; // eye to point after modelview
out vec3 c2p; // center to point after modelview

void main()
{	
	vec3 posi = 1.5 * position;
	c2p = (view * model * vec4(posi,0.0) ).xyz;
	e2p = (view * model * vec4(posi,1.0) ).xyz;
	vec4 pos = proj * view * model * vec4(posi, 1.0);
    gl_Position = logDepth(pos);
}
