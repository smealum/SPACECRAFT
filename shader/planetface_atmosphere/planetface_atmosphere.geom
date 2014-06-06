#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform vec3 lightdir;

in vec3 pos[];
in vec4 gcolor[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];

out vec4 fcolor;
out vec3 vPos;

void main()
{
	// on n'affiche plus les blocs qui sont de l'autre côté de la planete
	vec4 PlanetCenter = view*model*vec4(0.0,0.0,0.0,1.0);
	vec4 BlocPosition = view*model*vec4(pos[0],1.0);
	if ( dot(BlocPosition,normalize(PlanetCenter)) > length(PlanetCenter) )
		return;

	
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];

	vec4 vu[8];
	vec4 vp[8];
	float c[5];

	vu[0]=vec4(gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	vu[1]=vec4(gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	vu[2]=vec4(gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	vu[3]=vec4(gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	vu[4]=vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	vu[5]=vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	vu[6]=vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	vu[7]=vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);

	c[0]=((dot(lightdir,normalize(vec3(vu[0])))+1.0)/2.0)/255.0;
	c[1]=((dot(lightdir,normalize(vec3(vu[1]-vu[0])))+1.0)/2.0)/255.0;
	c[2]=((dot(lightdir,normalize(vec3(vu[0]-vu[1])))+1.0)/2.0)/255.0;
	c[3]=((dot(lightdir,normalize(vec3(vu[0]-vu[2])))+1.0)/2.0)/255.0;
	c[4]=((dot(lightdir,normalize(vec3(vu[2]-vu[0])))+1.0)/2.0)/255.0;
	
	mat4 projViewModel = proj*view*model;
	vp[0]=projViewModel*vu[0];
	vp[1]=projViewModel*vu[1];
	vp[2]=projViewModel*vu[2];
	vp[3]=projViewModel*vu[3];
	vp[4]=projViewModel*vu[4];
	vp[5]=projViewModel*vu[5];
	vp[6]=projViewModel*vu[6];
	vp[7]=projViewModel*vu[7];

	/*

	  6-------7
	 /|      /|
	4-------5 |
	| |     | |
	| 2-----|-3
	|/      |/
	0-------1

	*/

	fcolor.a = 1.0;

	fcolor.rgb = gcolor[0].rgb * vec3(84,124,60) * c[0];
	
	// TOP FACE
	vPos = vec3(vu[5]);
	gl_Position = logDepth(vp[5]); EmitVertex();
	vPos = vec3(vu[4]);
	gl_Position = logDepth(vp[4]); EmitVertex();
	vPos = vec3(vu[7]);
	gl_Position = logDepth(vp[7]); EmitVertex();
	vPos = vec3(vu[6]);
	gl_Position = logDepth(vp[6]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[1];

	// LEFT FACE
	vPos = vec3(vu[0]);
	gl_Position = logDepth(vp[0]); EmitVertex();
	vPos = vec3(vu[2]);
	gl_Position = logDepth(vp[2]); EmitVertex();
	vPos = vec3(vu[4]);
	gl_Position = logDepth(vp[4]); EmitVertex();
	vPos = vec3(vu[6]);
	gl_Position = logDepth(vp[6]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[2];

	// RIGHT FACE
	vPos = vec3(vu[3]);
	gl_Position = logDepth(vp[3]); EmitVertex();
	vPos = vec3(vu[1]);
	gl_Position = logDepth(vp[1]); EmitVertex();
	vPos = vec3(vu[7]);
	gl_Position = logDepth(vp[7]); EmitVertex();
	vPos = vec3(vu[5]);
	gl_Position = logDepth(vp[5]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[3];

	// FRONT FACE
	vPos = vec3(vu[1]);
	gl_Position = logDepth(vp[1]); EmitVertex();
	vPos = vec3(vu[0]);
	gl_Position = logDepth(vp[0]); EmitVertex();
	vPos = vec3(vu[5]);
	gl_Position = logDepth(vp[5]); EmitVertex();
	vPos = vec3(vu[4]);
	gl_Position = logDepth(vp[4]); EmitVertex();
	EndPrimitive();

	fcolor.rgb = gcolor[0].rgb * vec3(161,103,88) * c[4];

	// BACK FACE
	vPos = vec3(vu[2]);
	gl_Position = logDepth(vp[2]); EmitVertex();
	vPos = vec3(vu[3]);
	gl_Position = logDepth(vp[3]); EmitVertex();
	vPos = vec3(vu[6]);
	gl_Position = logDepth(vp[6]); EmitVertex();
	vPos = vec3(vu[7]);
	gl_Position = logDepth(vp[7]); EmitVertex();
	EndPrimitive();	
}
