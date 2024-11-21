#version 330 core

in vec2 vTexCoord; 
out vec4 FragColor; 

void main()
{
    float distanceFromCenter = length(vTexCoord - vec2(0.5, 0.5));
    float particleAlpha = smoothstep(0.45, 0.5, distanceFromCenter);

    FragColor = vec4(1.0, 1.0, 1.0, 0.7 * (1.0 - particleAlpha));
}
