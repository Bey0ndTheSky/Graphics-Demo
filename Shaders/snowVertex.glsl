#version 330 core

in vec3 position;
in vec2 TexCoord;
in vec3 instanceOffset;
uniform float gravity; 
uniform vec3 heightmapSize;
uniform sampler2D windMap;
uniform float windTranslate;
uniform float windStrength;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 vTexCoord;
void main()
{
	vec3 particlePos = position + instanceOffset;
	float particleSize = instanceOffset.y; 
	float startHeight = heightmapSize.y * 5.0; 
    float verticalFall = (startHeight - gravity * 6 * heightmapSize.y);
    particlePos.y = verticalFall;
	
    vec4 windSample = (texture(windMap, (instanceOffset.xz / heightmapSize.xz) + windTranslate) * 2 - 1);
	vec3 wind = normalize(vec3(windSample.r, 0.0, windSample.g));
	particlePos.xz += wind.xz * windStrength * heightmapSize.x;
	
	mat4 mvp = projMatrix * viewMatrix * modelMatrix; 
    gl_Position = mvp * vec4(particlePos, 1.0);
	gl_PointSize = particleSize;

    vTexCoord = particlePos.xz * 0.5 + 0.5;
}
