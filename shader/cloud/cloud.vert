#version 330

in vec3 position;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);
uniform float znear = -1.0;
uniform float zfar = 1.0;

out vec3 vPos;

void main()
{	
	vPos = position;
	vec4 pos = proj * view * model * vec4(1.03*normalize(position), 1.0);
	gl_Position = vec4(pos.xy,(2*log(pos.w/znear)/log(zfar/znear)-1)*pos.w,pos.w);
}
