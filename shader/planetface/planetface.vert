#version 150 core

in vec3 position;
in float elevation;
in float minElevation;
in float size;
in int topTile;
in int sideTile;

uniform mat4 model, view, proj;
uniform vec3 v1, v2;

out vec3 pos;
out vec3 gv1, gv2;
out float gelevation;
out float gminElevation;
out float gsize;
out int gtopTile;
out int gsideTile;

void main()
{
	

    pos=position;
    gv1=v1;
    gv2=v2;
    gelevation=elevation;
    gsize=size;
    gminElevation=minElevation;
	
	// 
	// gtopTile = topTile;
	// gsideTile = sideTile;

	if (elevation >1.001)
	{
		gtopTile = 0;
		gsideTile = 2;
	}
	else
	{
		gtopTile = 12*16+13;
		gsideTile = 12*16+13;
	}
}
