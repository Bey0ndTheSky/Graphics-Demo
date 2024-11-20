#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position; 
in vec2 texCoord; 
in vec4 colour;    
in vec3 normal;
in vec4 tangent; 
in vec3 instanceOffset;

out Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
} OUT;

void main(void) {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 wNormal = normalize(normalMatrix * normalize(normal));
    vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

    OUT.normal = wNormal;
    OUT.tangent = wTangent;
    OUT.binormal = cross(wTangent, wNormal) * tangent.w;
	mat4 mvp = projMatrix * viewMatrix * modelMatrix; 
    gl_Position = mvp * vec4(position + instanceOffset, 1.0);
    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
    OUT.colour = colour;
}
