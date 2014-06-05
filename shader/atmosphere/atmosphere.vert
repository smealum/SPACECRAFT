#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 position;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform float m_fOuterRadius;

out vec3 vPos;

void main()
{	
	vPos = position;
	
	vec4 v = proj * view * model * vec4(position*m_fOuterRadius, 1.0);

	gl_Position = logDepth(v);
}
