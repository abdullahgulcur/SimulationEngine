#version 420 core

layout (location = 0) in vec3 aPos;

uniform int patchRes;
uniform int mapSize;
uniform mat4 V;
uniform mat4 P;
uniform vec3 offset;
uniform int scale;
uniform sampler2D heightmap;

out vec3 WorldPos;
out vec2 TexCoords;

void main(void)
{
    vec3 pos = scale * patchRes * vec3((aPos / patchRes) + offset);
    vec2 texCoords = vec2(pos.x / mapSize, pos.z / mapSize);
    TexCoords = texCoords;
    float height = texture(heightmap, texCoords).r;
    pos.y = height;
    WorldPos = pos;
    gl_Position =  P * V * vec4(pos, 1.0);
}
