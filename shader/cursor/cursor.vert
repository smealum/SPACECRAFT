#define logDepth(v) vec4(v.xy,(log2(max(1e-6,1.0+v.w))*logconst-1.0)*v.w,v.w)

in vec3 offset_from_position;

uniform mat4 planetModel, model, view, proj;
uniform float logconst, zfar;
uniform vec3 position;
uniform vec3 origin, v1, v2;
uniform float numBlocks;

out vec3 fpos;

void main()
{
	vec3 pos1=position+vec3(model*vec4(offset_from_position,1.0));
	vec3 pos2=v1*pos1.x+v2*pos1.z;
	float y=1.0+pos1.y/numBlocks;

	fpos = offset_from_position;
	vec4 r = proj * view * planetModel * vec4((normalize(origin+(pos2)/numBlocks)*y),1.0);
	gl_Position = vec4(r.xy,-1.0*r.w,r.w);
}
