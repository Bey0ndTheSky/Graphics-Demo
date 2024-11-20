#version 330 core

uniform sampler2D diffuseTex;
uniform vec4 nodeColour;

in Vertex {
    vec2 texCoord;  
    vec4 colour; 
} IN;

out vec4 fragColour;

void main(void) {
    vec4 textureColour = texture(diffuseTex, IN.texCoord);
    
    if (length(IN.colour.rgb) > 0.1) {
        fragColour = mix(textureColour, IN.colour, 0.15);
    } 
	else {
        fragColour = textureColour;
    }   
	fragColour += nodeColour;
}
