#version 330

#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 model, view, proj;
uniform float logconst, zfar;

uniform vec3 v1, v2;
uniform vec3 origin;
uniform float numBlocks;

uniform vec3 lightdir;

in vec3 pos[];
in vec2 gtexcoord[];
in vec4 gcolor[];
in int dir[];

out vec4 fcolor;
out vec2 texcoord;

const vec3 o[]=vec3[](vec3(0.0,0.0,0.0), //bottom
						vec3(0.0,1.0,0.0), //top
						vec3(0.0,0.0,0.0), //left
						vec3(1.0,0.0,1.0), //right
						vec3(1.0,0.0,0.0), //near
						vec3(0.0,0.0,1.0), //far
						vec3(0.0,0.0,0.0), //diag11
						vec3(1.0,0.0,1.0), //diag12
						vec3(1.0,0.0,0.0), //diag21
						vec3(0.0,0.0,1.0) //diag22
						);

const vec3 d1[]=vec3[](vec3(1.0,0.0,0.0), //bottom
						vec3(0.0,0.0,1.0), //top
						vec3(0.0,0.0,1.0), //left
						vec3(0.0,0.0,-1.0), //right
						vec3(-1.0,0.0,0.0), //near
						vec3(1.0,0.0,0.0), //far
						vec3(1.0,0.0,1.0), //diag11
						vec3(-1.0,0.0,-1.0), //diag12
						vec3(-1.0,0.0,1.0), //diag21
						vec3(1.0,0.0,-1.0) //diag22
						);

const vec3 d2[]=vec3[](vec3(0.0,0.0,1.0), //bottom
						vec3(1.0,0.0,0.0), //top
						vec3(0.0,1.0,0.0), //left
						vec3(0.0,1.0,0.0), //right
						vec3(0.0,1.0,0.0), //near
						vec3(0.0,1.0,0.0), //far
						vec3(0.0,1.0,0.0), //diag11
						vec3(0.0,1.0,0.0), //diag12
						vec3(0.0,1.0,0.0), //diag21
						vec3(0.0,1.0,0.0) //diag22
						);

const vec3 n[]=vec3[](vec3(0.0,-1.0,0.0), //bottom
						vec3(0.0,1.0,0.0), //top
						vec3(-1.0,0.0,0.0), //left
						vec3(1.0,0.0,1.0), //right
						vec3(0.0,0.0,-1.0), //near
						vec3(0.0,0.0,1.0), //far
						normalize(vec3(1.0,0.0,-1.0)), //diag11
						normalize(vec3(-1.0,0.0,1.0)), //diag12
						normalize(vec3(1.0,0.0,1.0)), //diag21
						normalize(vec3(-1.0,0.0,-1.0)) //diag22
						);

void main()
{
	vec4 r;
	vec3 pos1, pos2;
	float y;


	pos1=pos[0]+o[dir[0]];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/numBlocks;

	vec3 rn=(normalize(origin+(pos2)/numBlocks)*y);

	const float ambient=0.0;
	float col=max(dot(vec3(normalize(n[dir[0]].x*v1+n[dir[0]].z*v2+n[dir[0]].y*rn)),lightdir),0.0)+ambient; //suit pas parfaitement la rotondité mais devrait suffire

	fcolor = vec4(vec3(col),1.0);
	r = proj * view * model * vec4(rn,1.0);
	gl_Position = logDepth(r);
	// gl_Position = r;
	texcoord=gtexcoord[0]+vec2(1,1)/16;
	EmitVertex();


	pos1=pos[0]+o[dir[0]]+d1[dir[0]];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/numBlocks;

	fcolor = vec4(vec3(col),1.0);
	r = proj * view * model * vec4((normalize(origin+(pos2)/numBlocks)*y),1.0);
	gl_Position = logDepth(r);
	// gl_Position = r;
	texcoord=gtexcoord[0]+vec2(0,1)/16;
	EmitVertex();


	pos1=pos[0]+o[dir[0]]+d2[dir[0]];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/numBlocks;

	fcolor = vec4(vec3(col),1.0);
	r = proj * view * model * vec4((normalize(origin+(pos2)/numBlocks)*y),1.0);
	gl_Position = logDepth(r);
	// gl_Position = r;
	texcoord=gtexcoord[0]+vec2(1,0)/16;
	EmitVertex();


	pos1=pos[0]+o[dir[0]]+d1[dir[0]]+d2[dir[0]];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/numBlocks;

	fcolor = vec4(vec3(col),1.0);
	r = proj * view * model * vec4((normalize(origin+(pos2)/numBlocks)*y),1.0);
	gl_Position = logDepth(r);
	// gl_Position = r;
	texcoord=gtexcoord[0];
	EmitVertex();
	EndPrimitive();
}
