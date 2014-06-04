#version 330

in vec3 position;

uniform mat4 model, view, proj;
uniform float znear, zfar;
uniform float m_fOuterRadius;

out vec3 vPos;

void main()
{	
	vPos = position;
	vec4 v = proj * view * model * vec4(position*m_fOuterRadius, 1.0);
	gl_Position = vec4(v.xy,(2*log(v.w/znear)/log(zfar/znear)-1)*v.w,v.w);
}
