#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D metallicRoughTex;

uniform vec3 cameraPosition;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec4 nodeColour;

in Vertex {
    vec2 texCoord;
    vec4 colour;
	vec3 normal;
    vec3 tangent; 
    vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main(void) {
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPosition - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    mat3 TBN = mat3(
        normalize(IN.tangent),
        normalize(IN.binormal),
        normalize(IN.normal)
    );

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
	if (length(nodeColour.rgb) > 0.1) {diffuse = nodeColour;}
	vec2 metallicRoughness = texture(metallicRoughTex, IN.texCoord).rg;
    float metallic = clamp(metallicRoughness.r, 0.0, 1.0);  
    float roughness = clamp(metallicRoughness.g, 0.05, 1.0);
	float smoothness = 1.0 - roughness;  
    vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
	
	vec3 F0 = mix(vec3(0.04), diffuse.rgb, metallic);     
    vec3 fresnel = fresnelSchlick(max(dot(viewDir, bumpNormal), 0.0), F0);

    float lambert = max(dot(incident, bumpNormal), 0.0);
	vec3 diffuseLight = diffuse.rgb * lambert;

    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    float NDF = pow(max(dot(bumpNormal, halfDir), 0.0), smoothness * 128.0); 
    float G = max(dot(bumpNormal, incident), 0.0) * max(dot(bumpNormal, viewDir), 0.0);
    vec3 specularLight = (NDF * G * fresnel) / max(lambert, 0.001);

    vec3 surface = (diffuseLight + specularLight) * attenuation;
    fragColour.rgb = surface + diffuse.rgb * 0.2;
    fragColour.a = diffuse.a;
}
