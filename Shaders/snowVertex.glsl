#version 330 core

in vec3 position;
in vec2 TexCoord;
in vec3 instanceOffset;
uniform float gravity; 
uniform sampler2D windMap;
uniform float windTranslate;
uniform float windStrength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 vTexCoord;
void main()
{
	vec3 particlePos = position;
	float particleSize = instanceOffset.y;
    particlePos.y -= gravity * 0.05; 

    vec4 windSample = (texture(windMap, TexCoord / 10.24 + windTranslate) * 2 - 1);
	vec3 wind = normalize(vec3(windSample.r, 0.0, windSample.g));
	particlePos.xz += wind.xz * windStrength;
	
	mat4 mvp = projMatrix * viewMatrix * modelMatrix; 
    gl_Position = mvp * vec4(particlePos, 1.0);
	gl_PointSize = particleSize;

    // Pass the texture coordinates for the particle (used for particle image or effect)
    vTexCoord = position.xz * 0.5 + 0.5; // Normalize the position
}
