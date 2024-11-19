#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;
uniform vec3 cameraPos;
uniform bool ice;

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    vec3 viewDir = normalize(cameraPos - IN.worldPos);

    vec3 reflectDir = reflect(-viewDir, normalize(IN.normal));

    vec4 reflectTex = texture(cubeTex, reflectDir);
	
	if (diffuse.a == 0.0f){fragColour = reflectTex;}
	else if (diffuse.a == 1.0f){fragColour = diffuse;}
	else{
		if(!ice){
		fragColour = reflectTex + (diffuse * 0.25f);
		fragColour.a = 0.55f;
		}
		else{
			fragColour = reflectTex + (diffuse * 0.1f);
			fragColour = mix(vec4(0.7, 0.9, 1.0, 1.0), fragColour, 0.6f);
			fragColour.a = 0.8f;
		}
	}
}
