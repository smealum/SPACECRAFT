#version 330

layout(points) in;
layout(points, max_vertices = 1) out;
in vec4 Color[];

out vec4 fcolor;

void main() {
        fcolor = Color[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}
