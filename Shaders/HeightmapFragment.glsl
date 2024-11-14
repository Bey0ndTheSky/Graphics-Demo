#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
    vec2 texCoord;  
    vec4 colour; 
	vec3 normal;
    vec3 tangent; 
    vec3 binormal; 
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
    // Sample the texture color
    vec4 textureColour = texture(diffuseTex, IN.texCoord);
    
    if (length(IN.colour.rgb) > 0.1) {
        fragColour = mix(textureColour, IN.colour, 0.15);
    } else {
        fragColour = textureColour;
    }   
}
