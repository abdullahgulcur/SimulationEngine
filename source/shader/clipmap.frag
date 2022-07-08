#version 330 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D heightmap;
uniform sampler2D grass_a_tex;
uniform sampler2D grass_n_tex;
uniform usampler2D colormap;

uniform vec3 color_d; // fucker lines
uniform vec3 lightDir;
uniform vec3 camPos;
uniform vec3 lightColor;

uniform int scale;
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

void main(){

    int viewportX = 4;
    int viewportY = 4;

    vec2 texcood00 = vec2(floor(WorldPos.x) / mapSize, floor(WorldPos.z) / mapSize); 

    float size = 25;
    vec2 texcood00_ = vec2((floor(WorldPos.x / size) * size) / mapSize, (floor(WorldPos.z / size) * size) / mapSize); 
    vec2 texcood10_ = vec2(((floor(WorldPos.x / size) * size) + size) / mapSize, (floor(WorldPos.z / size) * size) / mapSize); 
    vec2 texcood01_ = vec2((floor(WorldPos.x / size) * size) / mapSize, ((floor(WorldPos.z / size) * size) + size) / mapSize);  
    vec2 texcood11_ = vec2(((floor(WorldPos.x / size) * size) + size) / mapSize, ((floor(WorldPos.z / size) * size) + size) / mapSize);
    
    unsigned int paletteIndex00_ = texture(colormap, texcood00_).r;
    unsigned int paletteIndex10_ = texture(colormap, texcood10_).r; 
    unsigned int paletteIndex01_ = texture(colormap, texcood01_).r; 
    unsigned int paletteIndex11_ = texture(colormap, texcood11_).r;

    unsigned int paletteIndex00 = texture(colormap, texcood00).r;
    unsigned int paletteIndex10 = textureOffset(colormap, texcood00, ivec2(1, 0)).r; 
    unsigned int paletteIndex01 = textureOffset(colormap, texcood00, ivec2(0, 1)).r; 
    unsigned int paletteIndex11 = textureOffset(colormap, texcood00, ivec2(1, 1)).r;

    vec2 uv00;
    uv00.x = float(paletteIndex00 % viewportX) / viewportX  + fract(TexCoords.x * mapSize) / viewportX; // mapSize is actually tiling...
    uv00.y = float(paletteIndex00 / viewportY) / viewportY  + fract(TexCoords.y * mapSize) / viewportY;
    vec3 albedo_00 = texture(grass_a_tex, uv00).rgb;
    vec3 normal_00 = texture(grass_n_tex, uv00).rgb;

    vec2 uv10;
    uv10.x = float(paletteIndex10 % viewportX) / viewportX  + fract(TexCoords.x * mapSize) / viewportX;
    uv10.y = float(paletteIndex10 / viewportY) / viewportY  + fract(TexCoords.y * mapSize) / viewportY;
    vec3 albedo_10 = texture(grass_a_tex, uv10).rgb;
    vec3 normal_10 = texture(grass_n_tex, uv10).rgb;

    vec2 uv01;
    uv01.x = float(paletteIndex01 % viewportX) / viewportX  + fract(TexCoords.x * mapSize) / viewportX;
    uv01.y = float(paletteIndex01 / viewportY) / viewportY  + fract(TexCoords.y * mapSize) / viewportY;
    vec3 albedo_01 = texture(grass_a_tex, uv01).rgb;
    vec3 normal_01 = texture(grass_n_tex, uv01).rgb;

    vec2 uv11;
    uv11.x = float(paletteIndex11 % viewportX) / viewportX  + fract(TexCoords.x * mapSize) / viewportX;
    uv11.y = float(paletteIndex11 / viewportY) / viewportY  + fract(TexCoords.y * mapSize) / viewportY;
    vec3 albedo_11 = texture(grass_a_tex, uv11).rgb;
    vec3 normal_11 = texture(grass_n_tex, uv11).rgb;

    vec2 uv00_;
    uv00_.x = float(paletteIndex00_ % viewportX) / viewportX  + fract((TexCoords.x * mapSize) / size) / viewportX;
    uv00_.y = float(paletteIndex00_ / viewportY) / viewportY  + fract((TexCoords.y * mapSize) / size) / viewportY;
    vec3 albedo_00_ = texture(grass_a_tex, uv00_).rgb;
    vec3 normal_00_ = texture(grass_n_tex, uv00_).rgb;

    vec2 uv10_;
    uv10_.x = float(paletteIndex10_ % viewportX) / viewportX  + fract((TexCoords.x * mapSize) / size) / viewportX;
    uv10_.y = float(paletteIndex10_ / viewportY) / viewportY  + fract((TexCoords.y * mapSize) / size) / viewportY;
    vec3 albedo_10_ = texture(grass_a_tex, uv10_).rgb;
    vec3 normal_10_ = texture(grass_n_tex, uv10_).rgb;

    vec2 uv01_;
    uv01_.x = float(paletteIndex01_ % viewportX) / viewportX  + fract((TexCoords.x * mapSize) / size) / viewportX;
    uv01_.y = float(paletteIndex01_ / viewportY) / viewportY  + fract((TexCoords.y * mapSize) / size) / viewportY;
    vec3 albedo_01_ = texture(grass_a_tex, uv01_).rgb;
    vec3 normal_01_ = texture(grass_n_tex, uv01_).rgb;

    vec2 uv11_;
    uv11_.x = float(paletteIndex11_ % viewportX) / viewportX  + fract((TexCoords.x * mapSize) / size) / viewportX;
    uv11_.y = float(paletteIndex11_ / viewportY) / viewportY  + fract((TexCoords.y * mapSize) / size) / viewportY;
    vec3 albedo_11_ = texture(grass_a_tex, uv11_).rgb;
    vec3 normal_11_ = texture(grass_n_tex, uv11_).rgb;

    float deltaX = fract(WorldPos.x);
    float deltaY = fract(WorldPos.z);

    float deltaX_ = (WorldPos.x - floor(WorldPos.x / size) * size) / size;
    float deltaY_ = (WorldPos.z - floor(WorldPos.z / size) * size) / size;

    vec3 color = albedo_00 * (1 - deltaX) * (1 - deltaY) + albedo_10 * deltaX * (1 - deltaY) + albedo_01 * (1 - deltaX) * deltaY + albedo_11 * deltaX * deltaY;
    vec3 normal = normal_00 * (1 - deltaX) * (1 - deltaY) + normal_10 * deltaX * (1 - deltaY) + normal_01 * (1 - deltaX) * deltaY + normal_11 * deltaX * deltaY;

    vec3 color_ = albedo_00_ * (1 - deltaX_) * (1 - deltaY_) + albedo_10_ * deltaX_ * (1 - deltaY_) + albedo_01_ * (1 - deltaX_) * deltaY_ + albedo_11_ * deltaX_ * deltaY_;
    vec3 normal_ = normal_00_ * (1 - deltaX_) * (1 - deltaY_) + normal_10_ * deltaX_ * (1 - deltaY_) + normal_01_ * (1 - deltaX_) * deltaY_ + normal_11_ * deltaX_ * deltaY_;

    vec3 n = getNormalFromMap(normal.xyz, Normal);
    vec3 n_ = getNormalFromMap(normal_.xyz, Normal);

    //getNormal(TexCoords);

    float maxDist = 200;
    float minDist = 0;
    float alpha = clamp((distance(camPos, WorldPos) - minDist) / (maxDist - minDist), 0, 1);
    color = alpha * color_ + (1 - alpha) * color;
    n =  alpha * n_ + (1 - alpha) * n;

    vec3 ambient = 0.3 * color;
    float diff = max(dot(lightDir, n), 0.0);
    vec3 diffuse = diff * color;
    FragColor = vec4(ambient + diffuse, 1.0);
}