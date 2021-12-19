#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 color;

void main()
{
    float thickness = 0.01;
    float fade = 0.005;
    
    // Calculate distance and fill circle with white
    vec2 uv = TexCoords * 2.f - 1.f;
    float dist = 1.0 - length(uv);
    vec3 col = vec3(smoothstep(0.0, fade, dist));
    col *= vec3(smoothstep(thickness + fade, thickness, dist));

    if(col.r < 0.1f)
        discard;

    col *= color;

    FragColor = vec4(col, 1.0);
} 