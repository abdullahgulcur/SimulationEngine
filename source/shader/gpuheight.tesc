#version 420 core

layout(vertices=4) out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 mvp;

in block
{
    vec3 position;
	vec2 texCoord;
} In[];

out block
{
    vec3 position;
	vec2 texCoord;
} Out[];

float AdaptiveTessellation(vec3 p0, vec3 p1)
{
    vec3 center = (p0+p1)*0.5f;
	vec4 clip2 = view * vec4(center, 1.0);
	vec4 clip4 = view * vec4(0,0,0, 1.0);
	float cameraDist = length(clip2.xyz);
	float cameraDistFromCenter = length(clip4.xyz);

	vec4 clip0 = mvp * vec4(p0, 1.0);
	vec4 clip1 = mvp * vec4(p1, 1.0);
	const float d = distance(clip0, clip1);
	float terrTessTriSize = 32.0f;
	//return (1+clamp((cameraDistFromCenter*2/cameraDist)*(d / terrTessTriSize), 1, 61));
	return (clamp((256/cameraDist)*(d / terrTessTriSize), 1, 64));

}

void main()
{
    gl_TessLevelOuter[0] = AdaptiveTessellation(In[0].position, In[3].position);
	gl_TessLevelOuter[1] = AdaptiveTessellation(In[0].position, In[1].position);
	gl_TessLevelOuter[2] = AdaptiveTessellation(In[1].position, In[2].position);
	gl_TessLevelOuter[3] = AdaptiveTessellation(In[3].position, In[2].position);

	gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
	gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);

	Out[gl_InvocationID].position = In[gl_InvocationID].position;
	Out[gl_InvocationID].texCoord = In[gl_InvocationID].texCoord;
}