#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position; 
in vec2 texCoord; 
in vec4 colour;    
in vec3 instanceOffset;

out Vertex {
    vec2 texCoord;
    vec4 colour; 
} OUT;

void main(void) {
    mat4 mvp = projMatrix * viewMatrix * modelMatrix; 
	
    gl_Position = mvp * vec4(position + instanceOffset, 1.0);
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
    OUT.colour = colour;
}
