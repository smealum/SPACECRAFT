in vec4 fColor;

out vec4 outColor;

uniform float t;

in vec3 fpos;

void main()
{
  float f = 8.0*(fpos.y)*(1.0-fpos.y)*(fpos.z)*(1.0-fpos.z);
  f=sqrt(f);
  f=sqrt(f);
  f=sqrt(f);
    outColor.xyz = vec3(0.5+0.5*sin(10.0*t));
    // outColor.xyz = vec3((1.0-fpos.x)*(1.0+fpos.x)*(1.0-fpos.y)*(1.0+fpos.y));
  outColor.a = 1.0-f;
}
