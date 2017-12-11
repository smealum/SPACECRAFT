#define logDepthZ(v) ((log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w)
#define logDepth(v) vec4(v.xy,logDepthZ(v),v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;
uniform vec3 lightdir;
uniform vec3 planetPos;
uniform vec3 cameraPos;
uniform float size;

in vec3 pos[];
flat in int gtopTile[];
flat in int gsideTile[];
in vec3 gv1[];
in vec3 gv2[];
in float gelevation[];
in float gminElevation[];
in float gsize[];
in float grepeat[];

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

	vec4 v[8];
	float c[5];

	v[0]=model*vec4(gminElevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[1]=model*vec4(gminElevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[2]=model*vec4(gminElevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[3]=model*vec4(gminElevation[0]*normalize(pos[0]+v1+v2),1.0);
	v[4]=model*vec4(gelevation[0]*normalize(pos[0]-v1-v2),1.0);
	v[5]=model*vec4(gelevation[0]*normalize(pos[0]+v1-v2),1.0);
	v[6]=model*vec4(gelevation[0]*normalize(pos[0]-v1+v2),1.0);
	v[7]=model*vec4(gelevation[0]*normalize(pos[0]+v1+v2),1.0);

	// const float ambient=0.0;
	const float ambient=0.2;
	vec3 n1=normalize(vec3(v[0]-v[1]));
	vec3 n2=normalize(vec3(v[0]-v[2]));

	c[0]=ambient+max(dot(lightdir,normalize(vec3(v[0]))),0.0);
	c[1]=ambient+max(dot(lightdir,n1),0.0);
	c[2]=ambient+max(-dot(lightdir,n1),0.0);
	c[3]=ambient+max(dot(lightdir,n2),0.0);
	c[4]=ambient+max(-dot(lightdir,n2),0.0);

	bool drawLeft=dot(n1,vec3(v[0])-cameraPos)<0;
	bool drawFront=dot(n2,vec3(v[0])-cameraPos)<0;

    mat4 projview = proj * view;
    v[0] = projview * (vec4(planetPos,0.0)+v[0]);
    v[1] = projview * (vec4(planetPos,0.0)+v[1]);
    v[2] = projview * (vec4(planetPos,0.0)+v[2]);
    v[3] = projview * (vec4(planetPos,0.0)+v[3]);
    v[4] = projview * (vec4(planetPos,0.0)+v[4]);
    v[5] = projview * (vec4(planetPos,0.0)+v[5]);
    v[6] = projview * (vec4(planetPos,0.0)+v[6]);
    v[7] = projview * (vec4(planetPos,0.0)+v[7]);

	// XXX magique
    c[0] *= (gelevation[0]-1.00)*600.0/size;
    c[1] *= (gelevation[0]-1.00)*600.0/size;
    c[2] *= (gelevation[0]-1.00)*600.0/size;
    c[3] *= (gelevation[0]-1.00)*600.0/size;
    c[4] *= (gelevation[0]-1.00)*600.0/size;

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
	
}
