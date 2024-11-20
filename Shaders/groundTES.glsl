#version 400 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
} IN[];

out Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
} OUT;

uniform sampler2D DisplacementMap;
uniform vec3 hSize;
uniform float dispFactor;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

vec4 interpolate4D(vec4 v0, vec4 v1, vec4 v2)
{
    return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 + vec4(gl_TessCoord.z) * v2;
}


void main() {
    
	OUT.texCoord = interpolate2D(IN[0].texCoord, IN[1].texCoord, IN[2].texCoord);
	OUT.colour = interpolate4D(IN[0].colour, IN[1].colour, IN[2].colour);  
	OUT.normal = normalize(interpolate3D(IN[0].normal, IN[1].normal, IN[2].normal));
	OUT.tangent = normalize(interpolate3D(IN[0].tangent, IN[1].tangent, IN[2].tangent));
	OUT.binormal = normalize(interpolate3D(IN[0].binormal, IN[1].binormal, IN[2].binormal));
	OUT.worldPos = interpolate3D(IN[0].worldPos, IN[1].worldPos, IN[2].worldPos);
	
	float displacement = texture(DisplacementMap, OUT.texCoord.xy / 10.24).x;
    OUT.worldPos += OUT.worldPos * displacement * dispFactor;
		
    gl_Position = projMatrix * viewMatrix * vec4(OUT.worldPos, 1.0);
}
