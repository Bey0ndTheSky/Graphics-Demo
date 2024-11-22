#version 330 core

in vec2 vTexCoord; 
out vec4 FragColor; 
uniform sampler2D snowFlake;
void main()
{
    // Sample the texture for the snowflake
    vec4 texColor = texture(snowFlake, gl_PointCoord);

    // Discard fragments outside the particle texture's visible area (optional)
    if (texColor.a < 0.5) discard;

    // Output final color
    FragColor = texColor;
}
