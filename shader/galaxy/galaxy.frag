in float foffset;

uniform float t;

out vec4 outColor;

void main() {
    // outColor = vec4(1.0,1.0,1.0,0.8);
    outColor = vec4(1.0,1.0,1.0,0.5+cos(foffset+4.0*t)*0.25);
}
