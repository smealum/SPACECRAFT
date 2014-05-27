#version 330

smooth in vec4 fcolor;
smooth in vec3 fnormal;
smooth in vec4 fposition;
flat in vec3 flightPosition;
out vec4 outColor;

uniform float ambient = 0.1;
uniform float diffuse = 0.7;
uniform float specular = 0.3;

void main() {
    vec3 lightObjDirection = normalize(flightPosition-fposition.xyz);
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

	outColor.xyz=(  ambient+
                diffuse*coefDiffu+
                specular*coefSpecu)*fcolor.xyz;
    outColor.a=fcolor.a;
};
