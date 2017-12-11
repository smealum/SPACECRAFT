in vec3 vPos;

uniform float time;

out vec4 outColor;

in vec3 e2p; // eye to point after modelview
in vec3 c2p; // center to point after modelview

// version 3D cartésienne
void main()
{
	float s = dot(normalize(e2p),normalize(c2p));
	s=s*s;
	s=s*s;
	outColor = vec4(1.0,1.0,1.0,s);
}

