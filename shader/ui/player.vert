#version 330 core

in vec2 position;
in vec2 texcoord;

// block tile
uniform int tile;

// out to the fragment
flat out int ftile;
out vec2 ftexCoords;

void main()
{
    ftile = tile;
    ftexCoords = texcoord;
    gl_Position = vec4(position.xy, 0.0, 1.0);
}
