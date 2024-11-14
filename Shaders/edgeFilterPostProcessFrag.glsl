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
    float gradientX = 0.0;
    float gradientY = 0.0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            vec2 offset = vec2(i, j) * delta;
            vec4 texColor = texture(sceneTex, IN.texCoord.xy + offset);
            
            // Convert to grayscale
            float grayValue = (texColor.r + texColor.g + texColor.b) / 3.0; 
            
            gradientX += grayValue * Gx[i + 1][j + 1]; 
            gradientY += grayValue * Gy[i + 1][j + 1];
        }
    }
    
    float magnitude = sqrt(gradientX * gradientX + gradientY * gradientY);
    magnitude = clamp(magnitude, 0.0, 1.0);
   
    fragColor = vec4(magnitude, magnitude, magnitude, 1.0);
}
