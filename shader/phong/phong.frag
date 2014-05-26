#version 330

smooth in vec3 fcolor;
smooth in vec3 fnormal;
smooth in vec4 fposition;
flat in vec3 lightPosition;
out vec3 color;

uniform float ambient = 0.3;
uniform float diffuse = 0.3;
uniform float specular = 0.3;

void main() {
    vec3 lightObjDirection = normalize(lightPosition-fposition.xyz);
    vec3 normal = normalize(fnormal);
    float coefDiffu=dot(normal,lightObjDirection);
    float coefSpecu=dot(reflect(lightObjDirection,normal),normalize(fposition.xyz));
    if (coefDiffu<0.0) coefDiffu=0.0;
    if (coefSpecu<0.0) coefSpecu=0.0;
	coefSpecu=coefSpecu*coefSpecu;
	coefSpecu=coefSpecu*coefSpecu;
	coefSpecu=coefSpecu*coefSpecu;
	coefSpecu=coefSpecu*coefSpecu;
	coefSpecu*=7.0;

	color=( ambient+
		  	diffuse*coefDiffu+
		  	specular*coefSpecu)*fcolor;
};
