//#version 410 core
//
//in float Height;
//
//out vec4 FragColor;
//
//void main()
//{
//    float h = (Height + 16)/64.0f;
//    FragColor = vec4(h, h, h, 1.0);
//}
#version 420 core

out vec4 FragColor;
//in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec2 texCoord;

uniform sampler2D heightMap;

//#if DIR_LIGHT_COUNT != 0
//uniform vec3 dirLightDirections[DIR_LIGHT_COUNT];
//uniform vec3 dirLightColors[DIR_LIGHT_COUNT];
//uniform float dirLightPowers[DIR_LIGHT_COUNT];
//#endif
//
//#if POINT_LIGHT_COUNT != 0
//uniform vec3 pointLightPositions[POINT_LIGHT_COUNT];
//uniform vec3 pointLightColors[POINT_LIGHT_COUNT];
//uniform float pointLightPowers[POINT_LIGHT_COUNT];
//#endif

uniform vec3 camPos;

//const float PI = 3.14159265359;
//
//float DistributionGGX(vec3 N, vec3 H, float roughness)
//{
//    float a = roughness*roughness;
//    float a2 = a*a;
//    float NdotH = max(dot(N, H), 0.0);
//    float NdotH2 = NdotH*NdotH;
//
//    float nom   = a2;
//    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
//    denom = PI * denom * denom;
//
//    return nom / max(denom, 0.0000001);
//}
//float GeometrySchlickGGX(float NdotV, float roughness)
//{
//    float r = (roughness + 1.0);
//    float k = (r*r) / 8.0;
//
//    float nom   = NdotV;
//    float denom = NdotV * (1.0 - k) + k;
//
//    return nom / denom;
//}
//
//float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
//{
//    float NdotV = max(dot(N, V), 0.0);
//    float NdotL = max(dot(N, L), 0.0);
//    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
//    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
//
//    return ggx1 * ggx2;
//}
//
//vec3 fresnelSchlick(float cosTheta, vec3 F0)
//{
//    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
//}

void main()
{		
    // Compute Normal

    vec4 h;
    h.r = textureOffset(heightMap, texCoord, ivec2( 0,-1)).r;// * 256 + textureOffset(heightMap, texCoord, ivec2(0, -1)).g;
    h.g = textureOffset(heightMap, texCoord, ivec2(-1, 0)).r;// * 256 + textureOffset(heightMap, texCoord, ivec2(-1, 0)).g; 
    h.b	= textureOffset(heightMap, texCoord, ivec2( 1, 0)).r;// * 256 + textureOffset(heightMap, texCoord, ivec2(1, 0)).g; 
    h.a = textureOffset(heightMap, texCoord, ivec2( 0, 1)).r;// * 256 + textureOffset(heightMap, texCoord, ivec2(0, 1)).g; 

    vec3 n;
    n.z = h.r - h.a;
    n.x = h.g - h.b;
    n.y = 2;
    normalize(n);

    //vec3 va = normalize(vec3(2.0, (sright-sleft)* 8, 0.0)); 
    //vec3 vb = normalize(vec3(0.0, (stop-sbottom)* 8, -2.0)); 
    //vec3 N = cross(va,vb)*0.5+0.5; 


    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1,1,1);
  	
    // diffuse 
    //vec3 norm = normalize(vec3(N));
    //vec3 lightDir = normalize(vec3(0,1000,0) - vec3(WorldPos));
    vec3 lightDir = normalize(vec3(1,1,1));
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1,1,1) * 0.4;
    //vec3 diffuse = 0 * vec3(1,1,1);

    
    // specular
//    float specularStrength = 0.5;
//    vec3 viewDir = normalize(camPos - vec3(WorldPos));
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * vec3(1,1,1);  
        
    //vec3 result = (ambient + diffuse + specular) * vec3(1,0,0);
    vec3 result = (ambient + diffuse) * vec3(1,0,0);
    FragColor = vec4(result, 1.0);
//    vec3 albedo = vec3(1,0,0);
//    float metallic = 0.5;
//    float roughness = 0.5;
//    float ao = 0.2;
//    vec3 lightCol = vec3(1,1,1);
//    vec3 lightDir = vec3(0.8,0.5,0.3);
//    float lightPow = 100.f;
//
//    vec3 N = normalize(vec3(0,1,0));
//    vec3 V = normalize(camPos - vec3(WorldPos));
//  
//    vec3 F0 = vec3(0.04); 
//    F0 = mix(F0, albedo, metallic);
//
//    vec3 Lo = vec3(0.0);
//
//    vec3 L = normalize(lightDir);
//    vec3 H = normalize(V + L);
//    float attenuation = 1.0 / lightPow;
//    vec3 radiance = lightCol * 10;// * attenuation;
//    float NDF = DistributionGGX(N, H, roughness);   
//    float G   = GeometrySmith(N, V, L, roughness);      
//    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
//    vec3 numerator    = NDF * G * F; 
//    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
//    vec3 specular = numerator / max(denominator, 0.001);
//    vec3 kS = F;
//    vec3 kD = vec3(1.0) - kS;
//    kD *= 1.0 - metallic;
//    float NdotL = max(dot(N, L), 0.0);
//    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
//
////    #if DIR_LIGHT_COUNT != 0
////    for(int i = 0; i < DIR_LIGHT_COUNT; ++i) 
////    {
////        vec3 L = normalize(dirLightDirections[i]);
////        vec3 H = normalize(V + L);
////        float attenuation = 1.0 / dirLightPowers[i];
////        vec3 radiance = dirLightColors[i] * attenuation;
////        float NDF = DistributionGGX(N, H, roughness);   
////        float G   = GeometrySmith(N, V, L, roughness);      
////        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
////        vec3 numerator    = NDF * G * F; 
////        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
////        vec3 specular = numerator / max(denominator, 0.001);
////        vec3 kS = F;
////        vec3 kD = vec3(1.0) - kS;
////        kD *= 1.0 - metallic;
////        float NdotL = max(dot(N, L), 0.0);
////        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
////    }   
////    #endif
////
////    #if POINT_LIGHT_COUNT != 0
////    for(int i = 0; i < POINT_LIGHT_COUNT; ++i) 
////    {
////        vec3 L = normalize(pointLightPositions[i] - WorldPos);
////        vec3 H = normalize(V + L);
////        float distance = length(pointLightPositions[i] - WorldPos);
////        float attenuation = (1.0 * pointLightPowers[i]) / (distance * distance);
////        vec3 radiance = pointLightColors[i] * attenuation;
////        float NDF = DistributionGGX(N, H, roughness);   
////        float G   = GeometrySmith(N, V, L, roughness);      
////        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
////        vec3 numerator    = NDF * G * F; 
////        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
////        vec3 specular = numerator / max(denominator, 0.001);
////        vec3 kS = F;
////        vec3 kD = vec3(1.0) - kS;
////        kD *= 1.0 - metallic;
////        float NdotL = max(dot(N, L), 0.0);
////        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
////    }   
////    #endif
//
//    vec3 ambient = vec3(0.03) * albedo * ao;
//    vec3 color =  Lo;
//    color = color / (color + vec3(1.0));
//    color = pow(color, vec3(1.0/2.2));
//    FragColor = vec4(color, 1.0);
}
