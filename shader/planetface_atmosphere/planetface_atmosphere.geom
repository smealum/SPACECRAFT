#version 330

#define logDepthZ(v) ((log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w)
#define logDepth(v) vec4(v.xy,logDepthZ(v),v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform vec3 lightdir;
uniform vec3 planetPos;
uniform vec3 cameraPos;

in vec3 pos[];
flat in int gtopTile[];
flat in int gsideTile[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];
in float grepeat[];

out vec3 vPos;
out float fluminosity;
flat out int ftile;
out vec2 ftexCoords;
out float frepeat;

void main()
{
	// on n'affiche pas les blocs qui sont de l'autre côté de la planete
	vec4 PlanetCenter = view*vec4(planetPos,1.0);
	vec4 BlocPosition = view*(model*vec4(pos[0],1.0)+vec4(planetPos,0.0));
	if(dot(BlocPosition,normalize(PlanetCenter)) > length(PlanetCenter))return;

	frepeat = grepeat[0];
	
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];

	vec4 vu[8], vp[8];
	float c[5];

	vu[0]=model*vec4(gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	vu[1]=model*vec4(gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	vu[2]=model*vec4(gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	vu[3]=model*vec4(gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	vu[4]=model*vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	vu[5]=model*vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	vu[6]=model*vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	vu[7]=model*vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);

	const float ambient=0.0;
	vec3 n1=normalize(vec3(vu[0]-vu[1]));
	vec3 n2=normalize(vec3(vu[0]-vu[2]));

	c[0]=ambient+max(dot(lightdir,normalize(vec3(vu[0]))),0.0);
	c[1]=ambient+max(dot(lightdir,n1),0.0);
	c[2]=ambient+max(-dot(lightdir,n1),0.0);
	c[3]=ambient+max(dot(lightdir,n2),0.0);
	c[4]=ambient+max(-dot(lightdir,n2),0.0);

	bool drawLeft=dot(n1,vec3(vu[0])-cameraPos)<0;
	bool drawFront=dot(n2,vec3(vu[0])-cameraPos)<0;

	mat4 projview = proj * view;
	vp[0] = projview * (vec4(planetPos,0.0)+vu[0]);
	vp[1] = projview * (vec4(planetPos,0.0)+vu[1]);
	vp[2] = projview * (vec4(planetPos,0.0)+vu[2]);
	vp[3] = projview * (vec4(planetPos,0.0)+vu[3]);
	vp[4] = projview * (vec4(planetPos,0.0)+vu[4]);
	vp[5] = projview * (vec4(planetPos,0.0)+vu[5]);
	vp[6] = projview * (vec4(planetPos,0.0)+vu[6]);
	vp[7] = projview * (vec4(planetPos,0.0)+vu[7]);

	// // XXX temp
	// c[0] += (gelevation[0]-1.00)*400.0;
	// c[1] += (gelevation[0]-1.00)*400.0;
	// c[2] += (gelevation[0]-1.00)*400.0;
	// c[3] += (gelevation[0]-1.00)*400.0;
	// c[4] += (gelevation[0]-1.00)*400.0;

	/*
	  6-------7
	 /|      /|
	4-------5 |
	| |     | |
	| 2-----|-3
	|/      |/
	0-------1
	*/    

	ftile = gtopTile[0];

	// TOP FACE
	fluminosity = c[0];
	vPos = vec3(vu[5]); gl_Position = logDepth(vp[5]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	vPos = vec3(vu[4]); gl_Position = logDepth(vp[4]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	vPos = vec3(vu[7]); gl_Position = logDepth(vp[7]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	vPos = vec3(vu[6]); gl_Position = logDepth(vp[6]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();

	ftile = gsideTile[0];

	if(drawLeft)
	{
		// LEFT FACE
		fluminosity = c[1];
		vPos = vec3(vu[0]); gl_Position = logDepth(vp[0]); ftexCoords = vec2(1.0,1.0); EmitVertex();
		vPos = vec3(vu[2]); gl_Position = logDepth(vp[2]); ftexCoords = vec2(0.0,1.0); EmitVertex();
		vPos = vec3(vu[4]); gl_Position = logDepth(vp[4]); ftexCoords = vec2(1.0,0.0); EmitVertex();
		vPos = vec3(vu[6]); gl_Position = logDepth(vp[6]); ftexCoords = vec2(0.0,0.0); EmitVertex();
		EndPrimitive();
	}else{
		// RIGHT FACE
		fluminosity = c[2];
		vPos = vec3(vu[3]); gl_Position = logDepth(vp[3]); ftexCoords = vec2(1.0,1.0); EmitVertex();
		vPos = vec3(vu[1]); gl_Position = logDepth(vp[1]); ftexCoords = vec2(0.0,1.0); EmitVertex();
		vPos = vec3(vu[7]); gl_Position = logDepth(vp[7]); ftexCoords = vec2(1.0,0.0); EmitVertex();
		vPos = vec3(vu[5]); gl_Position = logDepth(vp[5]); ftexCoords = vec2(0.0,0.0); EmitVertex();
		EndPrimitive();
	}

	if(drawFront)
	{
		// FRONT FACE
		fluminosity = c[3];
		vPos = vec3(vu[1]); gl_Position = logDepth(vp[1]); ftexCoords = vec2(1.0,1.0); EmitVertex();
		vPos = vec3(vu[0]); gl_Position = logDepth(vp[0]); ftexCoords = vec2(0.0,1.0); EmitVertex();
		vPos = vec3(vu[5]); gl_Position = logDepth(vp[5]); ftexCoords = vec2(1.0,0.0); EmitVertex();
		vPos = vec3(vu[4]); gl_Position = logDepth(vp[4]); ftexCoords = vec2(0.0,0.0); EmitVertex();
		EndPrimitive();
	}else{
		// BACK FACE
		fluminosity = c[4];
		vPos = vec3(vu[2]); gl_Position = logDepth(vp[2]); ftexCoords = vec2(1.0,1.0); EmitVertex();
		vPos = vec3(vu[3]); gl_Position = logDepth(vp[3]); ftexCoords = vec2(0.0,1.0); EmitVertex();
		vPos = vec3(vu[6]); gl_Position = logDepth(vp[6]); ftexCoords = vec2(1.0,0.0); EmitVertex();
		vPos = vec3(vu[7]); gl_Position = logDepth(vp[7]); ftexCoords = vec2(0.0,0.0); EmitVertex();
		EndPrimitive();
	}
	
}
