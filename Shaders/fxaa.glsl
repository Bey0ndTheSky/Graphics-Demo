#version 330 core

uniform sampler2D sceneTex;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

// Sobel kernels for edge detection
const mat3 Gx = mat3(
    -1.0,  0.0,  1.0,
    -2.0,  0.0,  2.0,
    -1.0,  0.0,  1.0
);

const mat3 Gy = mat3(
    1.0,  2.0,  1.0,
    0.0,  0.0,  0.0,
   -1.0, -2.0, -1.0
);

void main(void) {
    vec2 delta = vec2(dFdx(IN.texCoord.x), dFdy(IN.texCoord.y));
    
	vec3 centerColor = texture(sceneTex, IN.texCoord).rgb;
    vec3 colorLeft = texture(sceneTex, IN.texCoord - vec2(delta.x, 0.0)).rgb;
    vec3 colorRight = texture(sceneTex, IN.texCoord + vec2(delta.x, 0.0)).rgb;
    vec3 colorUp = texture(sceneTex, IN.texCoord - vec2(0.0, delta.y)).rgb;
    vec3 colorDown = texture(sceneTex, IN.texCoord + vec2(0.0, delta.y)).rgb;

    // Calculate luminance for edge detection
    float centerLuminance = dot(centerColor, vec3(0.299, 0.587, 0.114)); // Grayscale luminance
    float lumLeft = dot(colorLeft, vec3(0.299, 0.587, 0.114));
    float lumRight = dot(colorRight, vec3(0.299, 0.587, 0.114));
    float lumUp = dot(colorUp, vec3(0.299, 0.587, 0.114));
    float lumDown = dot(colorDown, vec3(0.299, 0.587, 0.114));

    // Detect edges using luminance differences
    float edgeH = abs(lumLeft - lumRight);
    float edgeV = abs(lumUp - lumDown);
    float edgeMagnitude = max(edgeH, edgeV); // Edge strength

    // Smooth edges based on edge strength
    float smoothing = clamp(1.0 - edgeMagnitude * 2.0, 0.0, 1.0);

    // Blend the surrounding pixels based on smoothing factor
    vec3 blendedColor = (colorLeft + colorRight + colorUp + colorDown) * 0.25;

    // Mix the center color with the blended color for anti-aliasing
    fragColor = vec4(mix(centerColor, blendedColor, smoothing), 1.0);
}
