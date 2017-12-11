layout(location = 0) in vec3 position;
layout(location = 1) in float offset;

out float foffset;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

uniform float logconst;
uniform float zfar;

uniform float t;

void main ()
{
    vec4 p = proj*view*model*vec4(position,1.0);
	gl_Position = vec4(p.xy,0.9*p.w,p.w);
	foffset=offset;
}
