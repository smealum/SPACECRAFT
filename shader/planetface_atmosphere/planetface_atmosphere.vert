#version 150 core

in vec3 position;
in float elevation;
in float minElevation;
in float size;

uniform mat4 model, view, proj;
uniform vec3 v1, v2;

out vec3 pos;
out vec4 gcolor;
out vec3 gv1, gv2;
out float gelevation;
out float gminElevation;
out float gsize;

void main()
{
    gcolor = vec4(vec3((elevation-1.0f)*1000),1.0);
    if (elevation>1.001f)
        gcolor = gcolor*vec4(1.0,1.0,1.0,1.0);
    else
        gcolor = gcolor*vec4(26,10,230,255)*(3.0/255.0);
    // gcolor=vec4(abs(v1),1.0);
    pos=position;
    gv1=v1;
    gv2=v2;
    gelevation=elevation;
    gsize=size;
    gminElevation=minElevation;
}