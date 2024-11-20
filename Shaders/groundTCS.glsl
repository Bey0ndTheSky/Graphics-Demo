#version 400 core

layout (vertices = 3) out; // Number of vertices in output patch

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
} OUT[];

uniform mat4 viewMatrix;
uniform vec3 cameraPosition;
uniform vec3 hSize;

float GetTessLevel(float distance0, float distance1)
{
	const float farThreshold = 40000000.0;
    const float nearThreshold = 10000000.0;
	
	float avgDistance = (distance0 + distance1) / 2.0;
	float lerpFactor;
    float tessLevel;

    if (avgDistance <= nearThreshold) {
		lerpFactor = (nearThreshold - avgDistance) / nearThreshold;
		lerpFactor = clamp(lerpFactor, 0.0, 1.0);
        tessLevel = mix(4.5, 6.0, lerpFactor);
    }
    else if (avgDistance <= farThreshold) {
        lerpFactor = (avgDistance - nearThreshold) / (farThreshold - nearThreshold);
		lerpFactor = clamp(lerpFactor, 0.0, 1.0);
        tessLevel = mix(4.5, 2.5, lerpFactor);
    } else {
        tessLevel = 2.5;
    }

    return tessLevel;
}

void main() {
	
	float distanceSquared0 = dot(cameraPosition - IN[0].worldPos, cameraPosition - IN[0].worldPos);
	float distanceSquared1 = dot(cameraPosition - IN[1].worldPos, cameraPosition - IN[1].worldPos);
	float distanceSquared2 = dot(cameraPosition - IN[2].worldPos, cameraPosition - IN[2].worldPos);

    gl_TessLevelOuter[0] = GetTessLevel(distanceSquared1, distanceSquared2);
    gl_TessLevelOuter[1] = GetTessLevel(distanceSquared2, distanceSquared0);
    gl_TessLevelOuter[2] = GetTessLevel(distanceSquared0, distanceSquared1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];

	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
    OUT[gl_InvocationID].colour = IN[gl_InvocationID].colour;
	//OUT[gl_InvocationID].colour = mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), (IN[gl_InvocationID].texCoord.x / 10.24) ) 
    //+ mix(vec4(0.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), (IN[gl_InvocationID].texCoord.y / 10.24));

	//OUT[gl_InvocationID].colour = gl_TessLevelInner[0] >= 4.5 ? mix(vec4(0.0, 1.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), (gl_TessLevelInner[0] - 4.5) / 1.5):
	//mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), (gl_TessLevelInner[0] - 2.5) / (4.5 - 2.5));
    OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
    OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;
    OUT[gl_InvocationID].binormal = IN[gl_InvocationID].binormal;
    OUT[gl_InvocationID].worldPos = IN[gl_InvocationID].worldPos;
    
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
