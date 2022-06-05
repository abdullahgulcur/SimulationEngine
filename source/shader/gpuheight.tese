#version 420 core
layout(quads, equal_spacing, cw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in block
{
    vec3 position;
	vec2 texCoord;
} In[];

out float Height;
out vec3 Normal;
out vec3 WorldPos;
out vec2 texCoord;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = In[0].texCoord;
    vec2 t01 = In[1].texCoord;
    vec2 t10 = In[2].texCoord;
    vec2 t11 = In[3].texCoord;

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    texCoord = (t1 - t0) * v + t0;

    Height = texture(heightMap, texCoord).r;

    vec3 p00 = In[0].position;
    vec3 p01 = In[1].position;
    vec3 p10 = In[2].position;
    vec3 p11 = In[3].position;

    vec3 uVec = p01 - p00;
    vec3 vVec = p10 - p00;
    vec3 normal = normalize(cross(vVec.xyz, uVec.xyz));

    Normal = normal;

    vec3 p0 = (p01 - p00) * u + p00;
    vec3 p1 = (p11 - p10) * u + p10;
    vec3 p = (p1 - p0) * v + p0 + normal * Height;
    p.y = 0;
    WorldPos = p;
    gl_Position = projection * view * model * vec4(p, 1);
}