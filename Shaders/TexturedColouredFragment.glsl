#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
    vec2 texCoord;  
    vec4 colour; 
} IN;

out vec4 fragColour;

void main(void) {
    // Sample the texture color
    vec4 textureColour = texture(diffuseTex, IN.texCoord);
    
    // Blend the texture color with the vertex color
    fragColour = textureColour + IN.colour;
}
