#version 330

in vec3 position;

uniform mat4 model, view, proj;

out vec4 vPos;

void main()
{	
	vPos = vec4(position, 1.0);
	gl_Position = proj * view * model * vec4(position, 1.0);
}
