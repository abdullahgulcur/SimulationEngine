
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

//material parameters

#if USE_ALBEDO
uniform sampler2D albedoMap;
#else
uniform vec3 albedoColor;
#endif

#if USE_NORMAL
uniform sampler2D normalMap;
#endif

#if USE_METALLIC
uniform sampler2D metallicMap;
#endif

#if USE_ROUGHNESS
uniform sampler2D roughnessMap;
#endif

#if USE_AO
uniform sampler2D aoMap;
#endif

#if USE_NORMAL
uniform float normal_amount;
#endif
uniform float metallic_amount;
uniform float roughness_amount;
uniform float ao_amount;

#if DIR_LIGHT_COUNT != 0
uniform vec3 dirLightDirections[DIR_LIGHT_COUNT];
uniform vec3 dirLightColors[DIR_LIGHT_COUNT];
uniform float dirLightPowers[DIR_LIGHT_COUNT];
#endif

#if POINT_LIGHT_COUNT != 0
uniform vec3 pointLightPositions[POINT_LIGHT_COUNT];
uniform vec3 pointLightColors[POINT_LIGHT_COUNT];
uniform float pointLightPowers[POINT_LIGHT_COUNT];
#endif

uniform vec3 camPos;

const float PI = 3.14159265359;

#if USE_NORMAL
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
#endif

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{		
    #if USE_ALBEDO
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    #else
    vec3 albedo = albedoColor;
    #endif

    #if USE_METALLIC
    float metallicTexColor = texture(metallicMap, TexCoords).r;
    float metallic = metallicTexColor + (1 - metallicTexColor) * metallic_amount;
    #else
    float metallic = metallic_amount;
    #endif

    #if USE_ROUGHNESS
    float roughnessTexColor = texture(roughnessMap, TexCoords).r;
    float roughness = roughnessTexColor + (1 - roughnessTexColor) * roughness_amount;
    #else
    float roughness = roughness_amount;
    #endif

    #if USE_AO
    float aoTexColor = texture(aoMap, TexCoords).r;
    float ao = aoTexColor + (1 - aoTexColor) * ao_amount;
    #else
    float ao = ao_amount;
    #endif

    #if USE_NORMAL
    vec3 N = getNormalFromMap();
    #else
    vec3 N = normalize(Normal);
    #endif
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 Lo = vec3(0.0);

    #if DIR_LIGHT_COUNT != 0
    for(int i = 0; i < DIR_LIGHT_COUNT; ++i) 
    {
        vec3 L = normalize(dirLightDirections[i]);
        vec3 H = normalize(V + L);
        float attenuation = dirLightPowers[i];
        vec3 radiance = dirLightColors[i] * attenuation;
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    #endif

    #if POINT_LIGHT_COUNT != 0
    for(int i = 0; i < POINT_LIGHT_COUNT; ++i) 
    {
        vec3 L = normalize(pointLightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLightPositions[i] - WorldPos);
        float attenuation = (1.0 * pointLightPowers[i]) / (distance * distance);
        vec3 radiance = pointLightColors[i] * attenuation;
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = numerator / denominator;
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    #endif
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    FragColor = vec4(color, 1.0);
}
