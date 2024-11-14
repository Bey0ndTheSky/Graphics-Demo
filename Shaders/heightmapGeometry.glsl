#version 330 core

layout(triangles) in;  
layout(triangle_strip, max_vertices = 12) out; 

in Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
} IN[3];

out Vertex {
    vec2 texCoord;  
    vec4 colour; 
	vec3 normal;
    vec3 tangent; 
    vec3 binormal; 
    vec3 worldPos;
} OUT;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPosition;

uniform float grassHeight;        // Height of the grass blades
uniform float bladeWidth;         // Width of each grass blade
uniform vec4 colorBase;  // First color (e.g., base color of grass)
uniform vec4 colorTop;  // Second color (e.g., top color of grass)
	
vec4 toClipSpace(vec3 coord){
	return projMatrix * viewMatrix * vec4(coord, 1.0);
}

float rand(vec3 vec){
	return fract(sin(dot(vec, vec3(12.9898, 78.233, 54.53))) * 43758.5453);
 }
void main(void) {

	gl_Position  = gl_in[0].gl_Position;
	OUT.texCoord = IN[0].texCoord;
    OUT.colour = IN[0].colour;
	EmitVertex();
 
	gl_Position  = gl_in[1].gl_Position;
	OUT.texCoord = IN[1].texCoord;
    OUT.colour = IN[1].colour; 
	EmitVertex();
	
	gl_Position  = gl_in[2].gl_Position;
	OUT.texCoord = IN[2].texCoord;
    OUT.colour = IN[2].colour; 
	EmitVertex();
	EndPrimitive();
	
	for (int i = 0; i < 3; ++i) { 

		vec3 heightNormal = normalize(IN[i].normal);
		
		float randValue = rand(IN[i].worldPos);
		float randomAngle = randValue * 2.0 * 3.14159;
		float c = cos(randomAngle);
		float s = sin(randomAngle);
	
		mat3 rotationYMatrix = mat3(
			c, 0.0, s,
			0.0, 1.0, 0.0,
			-s, 0.0, c
        );
		
		randomAngle = randValue * 0.25 * 3.14159;
		c = cos(randomAngle);
		s = sin(randomAngle);
		mat3 rotationXMatrix = mat3(
            1.0, 0.0, 0.0,
			0.0, c, -s,
			0.0, s, c
        );
		vec3 randomTangent = normalize(rotationYMatrix * rotationXMatrix * IN[i].tangent);
		
		float width = mix(bladeWidth * 0.5, bladeWidth * 2, rand(IN[i].worldPos.xzy));
		float height = mix(grassHeight * 0.5, grassHeight * 2, rand(IN[i].worldPos.zxy));
		
        vec3 baseLeft = IN[i].worldPos - randomTangent * (width / 2.0);  // Left side of the blade
        vec3 baseRight = IN[i].worldPos + randomTangent * (width / 2.0);  // Right side of the blade
        
        // Calculate the top positions of the grass blade (using the normal to lift it)
        vec3 top = IN[i].worldPos + normalize(rotationYMatrix * rotationXMatrix * heightNormal) * height;
		OUT.texCoord = IN[i].texCoord;
		
		vec3 cameraToVertex = IN[i].worldPos - cameraPosition;
		vec3 viewDirection = normalize(cameraToVertex);
		float frontFace = dot(viewDirection, IN[0].normal);
		
		if (frontFace < 0.0){
			baseLeft = baseLeft + baseRight;
			baseRight = baseLeft - baseRight;
			baseLeft = baseLeft - baseRight;
		}
		
		gl_Position = toClipSpace(top);
		OUT.colour = colorTop;
		EmitVertex();
		
		gl_Position = toClipSpace(baseRight);
		OUT.colour = colorBase;
		EmitVertex();
		
		gl_Position = toClipSpace(baseLeft);
		EmitVertex();
		
		EndPrimitive();
	}
}
