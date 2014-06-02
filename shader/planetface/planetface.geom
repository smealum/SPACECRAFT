#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float znear, zfar;

in vec3 pos[];
in vec4 gcolor[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];

out vec4 fcolor;

void main()
{
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];
	vec4 v[8];

	mat4 projViewModel = proj*view*model;
	v[0]= projViewModel * vec4(gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[1]= projViewModel * vec4(gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[2]= projViewModel * vec4(gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[3]= projViewModel * vec4(gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	v[4]= projViewModel * vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[5]= projViewModel * vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[6]= projViewModel * vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[7]= projViewModel * vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);


/*

  6--------7
 /|       /|
4--------5 |
| |      | |
| 2------|-3
|/       |/
0--------1

*/

	fcolor.a = 1.0;

	fcolor.rgb = gcolor[0].rgb * vec3(0.2,0.9,0.2);
	
	// TOP FACE
	gl_Position = vec4(v[5].xy,(2*log(v[5].w/znear)/log(zfar/znear)-1)*v[5].w,v[5].w);	EmitVertex();
	gl_Position = vec4(v[4].xy,(2*log(v[4].w/znear)/log(zfar/znear)-1)*v[4].w,v[4].w);	EmitVertex();
	gl_Position = vec4(v[7].xy,(2*log(v[7].w/znear)/log(zfar/znear)-1)*v[7].w,v[7].w);	EmitVertex();
	gl_Position = vec4(v[6].xy,(2*log(v[6].w/znear)/log(zfar/znear)-1)*v[6].w,v[6].w);	EmitVertex();
	EndPrimitive();


	fcolor.rgb = gcolor[0].rgb * vec3(0.7,0.5,0.2);

	// LEFT FACE
	gl_Position = vec4(v[0].xy,(2*log(v[0].w/znear)/log(zfar/znear)-1)*v[0].w,v[0].w);	EmitVertex();
	gl_Position = vec4(v[2].xy,(2*log(v[2].w/znear)/log(zfar/znear)-1)*v[2].w,v[2].w);	EmitVertex();
	gl_Position = vec4(v[4].xy,(2*log(v[4].w/znear)/log(zfar/znear)-1)*v[4].w,v[4].w);	EmitVertex();
	gl_Position = vec4(v[6].xy,(2*log(v[6].w/znear)/log(zfar/znear)-1)*v[6].w,v[6].w);	EmitVertex();
	EndPrimitive();

	// RIGHT FACE
	gl_Position = vec4(v[3].xy,(2*log(v[3].w/znear)/log(zfar/znear)-1)*v[3].w,v[3].w);	EmitVertex();
	gl_Position = vec4(v[1].xy,(2*log(v[1].w/znear)/log(zfar/znear)-1)*v[1].w,v[1].w);	EmitVertex();
	gl_Position = vec4(v[7].xy,(2*log(v[7].w/znear)/log(zfar/znear)-1)*v[7].w,v[7].w);	EmitVertex();
	gl_Position = vec4(v[5].xy,(2*log(v[5].w/znear)/log(zfar/znear)-1)*v[5].w,v[5].w);	EmitVertex();
	EndPrimitive();

	// FRONT FACE
	gl_Position = vec4(v[1].xy,(2*log(v[1].w/znear)/log(zfar/znear)-1)*v[1].w,v[1].w);	EmitVertex();
	gl_Position = vec4(v[0].xy,(2*log(v[0].w/znear)/log(zfar/znear)-1)*v[0].w,v[0].w);	EmitVertex();
	gl_Position = vec4(v[5].xy,(2*log(v[5].w/znear)/log(zfar/znear)-1)*v[5].w,v[5].w);	EmitVertex();
	gl_Position = vec4(v[4].xy,(2*log(v[4].w/znear)/log(zfar/znear)-1)*v[4].w,v[4].w);	EmitVertex();
	EndPrimitive();

	// BACK FACE
	gl_Position = vec4(v[2].xy,(2*log(v[2].w/znear)/log(zfar/znear)-1)*v[2].w,v[2].w);	EmitVertex();
	gl_Position = vec4(v[3].xy,(2*log(v[3].w/znear)/log(zfar/znear)-1)*v[3].w,v[3].w);	EmitVertex();
	gl_Position = vec4(v[6].xy,(2*log(v[6].w/znear)/log(zfar/znear)-1)*v[6].w,v[6].w);	EmitVertex();
	gl_Position = vec4(v[7].xy,(2*log(v[7].w/znear)/log(zfar/znear)-1)*v[7].w,v[7].w);	EmitVertex();
	EndPrimitive();
	
}
