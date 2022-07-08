#version 420 core

layout (location = 0) in vec3 aPos;

uniform int patchRes;
uniform int mapSize;
uniform mat4 V;
uniform mat4 P;
uniform vec3 offset;
uniform int scale;
uniform sampler2D heightmap;
uniform sampler2D normalmap;

out vec3 WorldPos;
out vec2 TexCoords;
out vec3 Normal;

void main(void)
{
    vec3 pos = scale * patchRes * vec3((aPos / patchRes) + offset);
    vec2 texCoords = vec2(pos.x / mapSize, pos.z / mapSize);
    TexCoords = texCoords;
    Normal = texture(normalmap, TexCoords).rgb;
    //Normal = vec3(0,1,0);
    float height = texture(heightmap, texCoords).r;
    pos.y = height;
    WorldPos = pos;
    gl_Position =  P * V * vec4(pos, 1.0);
}
