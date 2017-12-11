smooth in vec4 fcolor;
smooth in vec3 fnormal;
smooth in vec3 flightDir;
smooth in vec3 feyeDir;
out vec4 outColor;

uniform float ambient = 0.3;
uniform float diffuse = 0.7;
uniform float specular = 1.0;
uniform float specPow = 1.0;


void main() {
    outColor = ambient * fcolor;

    vec3 N = normalize(fnormal);
    vec3 L = normalize(flightDir);

    float lambertTerm = dot(N,L);
    if (lambertTerm > 0.0)
    {
        outColor += diffuse * lambertTerm * fcolor;
        
        vec3 E = normalize(feyeDir);
        vec3 R = reflect(-L,N);

        outColor += specular * pow( max(dot(E,R),0.0), specPow) * fcolor;
    }

    outColor.a = fcolor.a;

}
