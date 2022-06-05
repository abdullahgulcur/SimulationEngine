#version 420 core

uniform float scalar;
uniform int size;

uniform sampler2D heightMap;

out block
{
	vec3 position;
	vec2 texCoord;
} Out;

void main()
{	 
	int halfSize = size / 2;
	int z = gl_VertexID / size;
	int x = gl_VertexID % size;
	Out.texCoord = vec2(x / (size - 1), 1 - z / (size - 1));
	vec4 height = texture2D(heightMap, Out.texCoord);
    Out.position = vec3((x - halfSize) * scalar, height.r, (z - halfSize) * scalar);
}
