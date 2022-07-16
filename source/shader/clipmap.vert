#version 420 core

layout (location = 0) in vec3 aPos;

//uniform int patchRes;
//uniform vec3 offset;
uniform vec3 offsetMultPatchRes;
uniform int mapSize;
//uniform mat4 V;
//uniform mat4 P;
uniform mat4 PV;
uniform float scale;
uniform sampler2D heightmap;
uniform sampler2D normalmap;

out vec3 WorldPos;
out vec2 TexCoords;
out vec3 Normal;

void main(void)
{
    vec3 pos = scale * (aPos + offsetMultPatchRes);
    vec2 texCoords = vec2(pos.x / mapSize, pos.z / mapSize);
    Normal = texture(normalmap, texCoords).rgb;
    float height = texture(heightmap, texCoords).r;
    pos.y = height;

    WorldPos = pos;
    TexCoords = texCoords * mapSize;

    gl_Position =  PV * vec4(pos, 1.0);
}
