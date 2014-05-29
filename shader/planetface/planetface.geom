#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 model, view, proj;
uniform float znear, zfar;

in vec3 pos[];
in vec4 gcolor[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gsize[];

out vec4 fcolor;

void main()
{
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];
	vec4 r;

	fcolor = gcolor[0];
	r = proj * view * model * vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	gl_Position = vec4(r.xy,(2*log(r.w/znear)/log(zfar/znear)-1)*r.w,r.w);
	EmitVertex();

	fcolor = gcolor[0];
	r = proj * view * model * vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	gl_Position = vec4(r.xy,(2*log(r.w/znear)/log(zfar/znear)-1)*r.w,r.w);
	EmitVertex();

	fcolor = gcolor[0];
	r = proj * view * model * vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);
	gl_Position = vec4(r.xy,(2*log(r.w/znear)/log(zfar/znear)-1)*r.w,r.w);
	EmitVertex();

	fcolor = gcolor[0];
	r = proj * view * model * vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	gl_Position = vec4(r.xy,(2*log(r.w/znear)/log(zfar/znear)-1)*r.w,r.w);
	EmitVertex();
	EndPrimitive();
}
