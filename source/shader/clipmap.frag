#version 330 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D heightmap;
uniform sampler2D grass_a_tex;
uniform sampler2D grass_n_tex;
uniform usampler2D colormap;

//uniform vec3 color_d; // fucker lines
uniform vec3 lightDir;
uniform vec3 camPos;
//uniform vec3 lightColor;

uniform int mapSize;

vec3 getNormalFromMap(vec3 normalColor, vec3 N)
{
    vec3 tangentNormal = normalColor * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

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

float getMipLevel(vec2 uv, vec2 texSize){

    vec2 dx = dFdx(uv *  texSize.x);
    vec2 dy = dFdy(uv *  texSize.y);
    float d = max(dot(dx, dx), dot(dy,dy));
    return 0.5 * log2(d);
}

void main(){

    int viewportX = 4;
    int viewportY = 4;

    vec2 texcood00 = vec2(floor(WorldPos.x) / mapSize, floor(WorldPos.z) / mapSize); 

    
    unsigned int paletteIndex00 = texture(colormap, texcood00).r;
    unsigned int paletteIndex10 = textureOffset(colormap, texcood00, ivec2(1, 0)).r; 
    unsigned int paletteIndex01 = textureOffset(colormap, texcood00, ivec2(0, 1)).r; 
    unsigned int paletteIndex11 = textureOffset(colormap, texcood00, ivec2(1, 1)).r;

    vec2 uv00;
    uv00.x = float(paletteIndex00 % viewportX) / viewportX  + fract(TexCoords.x) / viewportX; // mapSize is actually tiling...
    uv00.y = float(paletteIndex00 / viewportY) / viewportY  + fract(TexCoords.y) / viewportY;
    float mipLevel00 = getMipLevel(TexCoords, vec2(mapSize / viewportX));
    vec3 albedo_00 = textureLod(grass_a_tex, uv00, mipLevel00).rgb;
    vec3 normal_00 = textureLod(grass_n_tex, uv00, mipLevel00).rgb;
    //vec3 albedo_00 = texture(grass_a_tex, uv00).rgb;
    //vec3 normal_00 = texture(grass_n_tex, uv00).rgb;

    vec2 uv10;
    uv10.x = float(paletteIndex10 % viewportX) / viewportX  + fract(TexCoords.x) / viewportX;
    uv10.y = float(paletteIndex10 / viewportY) / viewportY  + fract(TexCoords.y) / viewportY;
    float mipLevel10 = getMipLevel(TexCoords, vec2(mapSize / viewportX));
    vec3 albedo_10 = textureLod(grass_a_tex, uv10, mipLevel10).rgb;
    vec3 normal_10 = textureLod(grass_n_tex, uv10, mipLevel10).rgb;
//    vec2 mipLevel10 = textureQueryLod(grass_a_tex, uv10);
//    vec3 albedo_10 = texture(grass_a_tex, uv10).rgb;
//    vec3 normal_10 = texture(grass_n_tex, uv10).rgb;

    vec2 uv01;
    uv01.x = float(paletteIndex01 % viewportX) / viewportX  + fract(TexCoords.x) / viewportX;
    uv01.y = float(paletteIndex01 / viewportY) / viewportY  + fract(TexCoords.y) / viewportY;
    float mipLevel01 = getMipLevel(TexCoords, vec2(mapSize / viewportX));
    vec3 albedo_01 = textureLod(grass_a_tex, uv01, mipLevel01).rgb;
    vec3 normal_01 = textureLod(grass_n_tex, uv01, mipLevel01).rgb;
//    vec2 mipLevel01 = textureQueryLod(grass_a_tex, uv01);
//    vec3 albedo_01 = texture(grass_a_tex, uv01).rgb;
//    vec3 normal_01 = texture(grass_n_tex, uv01).rgb;

    vec2 uv11;
    uv11.x = float(paletteIndex11 % viewportX) / viewportX  + fract(TexCoords.x) / viewportX;
    uv11.y = float(paletteIndex11 / viewportY) / viewportY  + fract(TexCoords.y) / viewportY;
    float mipLevel11 = getMipLevel(TexCoords, vec2(mapSize / viewportX));
    vec3 albedo_11 = textureLod(grass_a_tex, uv11, mipLevel11).rgb;
    vec3 normal_11 = textureLod(grass_n_tex, uv11, mipLevel11).rgb;
//    vec2 mipLevel11 = textureQueryLod(grass_a_tex, uv11);
//    vec3 albedo_11 = texture(grass_a_tex, uv11).rgb;
//    vec3 normal_11 = texture(grass_n_tex, uv11).rgb;

    float deltaX = fract(WorldPos.x);
    float deltaY = fract(WorldPos.z);

    vec3 color = albedo_00 * (1 - deltaX) * (1 - deltaY) + albedo_10 * deltaX * (1 - deltaY) + albedo_01 * (1 - deltaX) * deltaY + albedo_11 * deltaX * deltaY;
    vec3 normal = normal_00 * (1 - deltaX) * (1 - deltaY) + normal_10 * deltaX * (1 - deltaY) + normal_01 * (1 - deltaX) * deltaY + normal_11 * deltaX * deltaY;

    vec3 n = getNormalFromMap(normal.xyz, Normal);

    vec3 ambient = 0.3 * color;
    float diff = max(dot(lightDir, n), 0.0);
    vec3 diffuse = diff * color;
    FragColor = vec4(ambient + diffuse, 1.0);
}