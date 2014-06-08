#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform vec3 lightdir;
uniform vec3 planetPos;

in vec3 pos[];
in int gtopTile[];
in int gsideTile[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];
in float grepeat[];

out float fluminosity;
out flat int ftile;
out vec2 ftexCoords;
out float frepeat;

void main()
{
	// on n'affiche plus les blocs qui sont de l'autre côté de la planete
	vec4 PlanetCenter = view*vec4(planetPos,1.0);
	vec4 BlocPosition = view*(model*vec4(pos[0],1.0)+vec4(planetPos,0.0));
	if ( dot(BlocPosition,normalize(PlanetCenter)) > length(PlanetCenter) )
		return;

	frepeat = grepeat[0];
	
	vec3 v1=gv1[0]*gsize[0];
	vec3 v2=gv2[0]*gsize[0];

	vec4 v[8];
	float c[5];

	v[0]=model*vec4(planetPos+gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[1]=model*vec4(planetPos+gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[2]=model*vec4(planetPos+gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[3]=model*vec4(planetPos+gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	v[4]=model*vec4(planetPos+gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[5]=model*vec4(planetPos+gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[6]=model*vec4(planetPos+gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[7]=model*vec4(planetPos+gelevation[0]*normalize(pos[0]+v1+v2),1.0);


	const float ambient=0.0;
	c[0]=max(dot(lightdir,normalize(vec3(v[0])-planetPos)),0.0)+ambient;
	c[1]=max(dot(lightdir,normalize(vec3(v[0]-v[1]))),0.0)+ambient;
	c[2]=max(dot(lightdir,normalize(vec3(v[1]-v[0]))),0.0)+ambient;
	c[3]=max(dot(lightdir,normalize(vec3(v[0]-v[2]))),0.0)+ambient;
	c[4]=max(dot(lightdir,normalize(vec3(v[2]-v[0]))),0.0)+ambient;

    mat4 projview = proj * view;
    v[0] = projview * v[0];
    v[1] = projview * v[1];
    v[2] = projview * v[2];
    v[3] = projview * v[3];
    v[4] = projview * v[4];
    v[5] = projview * v[5];
    v[6] = projview * v[6];
    v[7] = projview * v[7];

	// XXX temp
	/*c[0] += (gelevation[0]-1.0)*400.0;*/
	/*c[1] += (gelevation[0]-1.0)*400.0;*/
	/*c[2] += (gelevation[0]-1.0)*400.0;*/
	/*c[3] += (gelevation[0]-1.0)*400.0;*/
	/*c[4] += (gelevation[0]-1.0)*400.0;*/
	
    /*
	mat4 projView = proj*view;
	v[0]=projView*(vec4(planetPos,1.0)+v[0]);
	v[1]=projView*(vec4(planetPos,1.0)+v[1]);
	v[2]=projView*(vec4(planetPos,1.0)+v[2]);
	v[3]=projView*(vec4(planetPos,1.0)+v[3]);
	v[4]=projView*(vec4(planetPos,1.0)+v[4]);
	v[5]=projView*(vec4(planetPos,1.0)+v[5]);
	v[6]=projView*(vec4(planetPos,1.0)+v[6]);
	v[7]=projView*(vec4(planetPos,1.0)+v[7]);
    */

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
	gl_Position = logDepth(v[5]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	gl_Position = logDepth(v[4]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	gl_Position = logDepth(v[7]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	gl_Position = logDepth(v[6]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();

	ftile = gsideTile[0];

	// LEFT FACE
	fluminosity = c[1];
	gl_Position = logDepth(v[0]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	gl_Position = logDepth(v[2]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	gl_Position = logDepth(v[4]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	gl_Position = logDepth(v[6]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();


	// RIGHT FACE
	fluminosity = c[2];
	gl_Position = logDepth(v[3]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	gl_Position = logDepth(v[1]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	gl_Position = logDepth(v[7]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	gl_Position = logDepth(v[5]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();

	// FRONT FACE
	fluminosity = c[3];
	gl_Position = logDepth(v[1]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	gl_Position = logDepth(v[0]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	gl_Position = logDepth(v[5]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	gl_Position = logDepth(v[4]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();


	// BACK FACE
	fluminosity = c[4];
	gl_Position = logDepth(v[2]); ftexCoords = vec2(1.0,1.0); EmitVertex();
	gl_Position = logDepth(v[3]); ftexCoords = vec2(0.0,1.0); EmitVertex();
	gl_Position = logDepth(v[6]); ftexCoords = vec2(1.0,0.0); EmitVertex();
	gl_Position = logDepth(v[7]); ftexCoords = vec2(0.0,0.0); EmitVertex();
	EndPrimitive();
	
}
