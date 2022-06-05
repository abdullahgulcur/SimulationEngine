#version 330 core

in vec3 WorldPos;
in vec2 TexCoords;
out vec4 FragColor;

uniform vec2 TexCoord;
uniform vec3 color; // fucker lines
uniform sampler2D heightmap;
uniform sampler2D grass_a_tex;
uniform sampler2D grass_n_tex;
uniform int clipMapSize;
uniform int scale;

uniform vec3 lightDir;
uniform vec3 camPos;
uniform vec3 lightColor;

vec3 getNormalFromMap(vec2 uv, vec3 normal)
{
    vec3 tangentNormal = texture(grass_n_tex, uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 getNormal(vec2 texcoord){

    vec4 h;
    h[0] = textureOffset(heightmap, texcoord, ivec2( 0,-1)).x; 
	h[1] = textureOffset(heightmap, texcoord, ivec2(-1, 0)).x; 
    h[2] = textureOffset(heightmap, texcoord, ivec2( 1, 0)).x; 
    h[3] = textureOffset(heightmap, texcoord, ivec2( 0, 1)).x; 

    vec3 n;
    n.z = h[0] - h[3];
    n.x = h[1] - h[2];
    n.y = 2;

    return normalize(n);
}

void main(){

    vec2 uv = fract(TexCoords * clipMapSize / scale);
    vec3 normal = getNormalFromMap(uv, normalize(getNormal(TexCoords)));
    vec3 color = texture(grass_a_tex, uv).rgb;
    vec3 ambient = 0.3 * color;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    FragColor = vec4(ambient + diffuse, 1.0);
}