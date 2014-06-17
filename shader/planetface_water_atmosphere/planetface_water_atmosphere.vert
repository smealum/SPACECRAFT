#version 150 core

in vec3 position;
in float elevation;
in float minElevation;
in float size;
in int topTile;
in int sideTile;
in float repeat;

uniform mat4 model, view, proj;
uniform vec3 v1, v2;

out vec3 pos;
out vec3 gv1, gv2;
out float gelevation;
out float gminElevation;
out float gsize;
flat out int gtopTile;
flat out int gsideTile;
out float grepeat;

void main()
{
    pos=position;
    gv1=v1;
    gv2=v2;
    gelevation=elevation;
    gsize=size;
    gminElevation=minElevation;
    grepeat=repeat;
    gtopTile = topTile;
    gsideTile = sideTile;
}
