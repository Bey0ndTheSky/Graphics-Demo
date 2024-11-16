#version 330 core

uniform sampler2D diffuseTex;

in Vertex {
    vec2 texCoord;  
    vec4 colour; 
} IN;

out vec4 fragColour;

void main(void) {
    
    vec4 textureColour = texture(diffuseTex, IN.texCoord);
    
    fragColour = mix(vec4(0.0, 0.0, 0.0, 1.0),vec4(1.0, 0.0, 0.0, 1.0), IN.texCoord.x) 
	+ mix(vec4(0.0, 0.0, 0.0, 1.0),vec4(0.0, 1.0, 0.0, 1.0), IN.texCoord.y);
	
	fragColour = textureColour + IN.colour;
}
