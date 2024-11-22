#version 330 core

#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_SPAN_MAX     8.0

uniform sampler2D sceneTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
    vec2 delta = vec2(dFdx(IN.texCoord.x), dFdy(IN.texCoord.y));
    
	vec3 rgbM = texture(sceneTex, IN.texCoord).rgb;
    vec3 rgbNW = texture(sceneTex, IN.texCoord + vec2(delta.x, -delta.y)).rgb;
    vec3 rgbNE = texture(sceneTex, IN.texCoord + vec2(delta.x, delta.y)).rgb;
    vec3 rgbSW = texture(sceneTex, IN.texCoord + vec2(-delta.x, -delta.y)).rgb;
	vec3 rgbSE = texture(sceneTex, IN.texCoord + vec2(-delta.x, delta.y)).rgb;
	
	vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *
                          (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              dir * rcpDirMin)) * delta;
      
    vec3 rgbA = 0.5 * (
        texture2D(sceneTex, IN.texCoord + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture2D(sceneTex, IN.texCoord + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture2D(sceneTex, IN.texCoord + dir * -0.5).xyz +
        texture2D(sceneTex, IN.texCoord + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMin) || (lumaB > lumaMax))
        fragColor = vec4(rgbA, 1.0);
    else
        fragColor = vec4(rgbB, 1.0);
}
