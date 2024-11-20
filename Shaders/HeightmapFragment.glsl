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
uniform vec3 cameraPosition;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

void main(void) {
	vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPosition - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
	
	float lambert = max(dot(incident, IN.normal), 0.0f);
	float distance = length(lightPos - IN.worldPos);
	float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
	specFactor = pow(specFactor, 4.0);
	
	vec4 groundColour;
	if (length(IN.colour.rgb) > 0.1) {
			groundColour = mix(diffuse, IN.colour, 0.15);
		} 
	else {
		groundColour = diffuse;
	}   
    vec3 surface = (groundColour.rgb * lightColour.rgb);
    fragColour.rgb = surface * lambert * attenuation;
    fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.1;
    fragColour.rgb += surface * 0.2;
	
    fragColour.a = diffuse.a;
}
