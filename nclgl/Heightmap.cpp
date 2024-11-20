#include "HeightMap.h"
#include <iostream>
#include <algorithm>

HeightMap::HeightMap(const std::string& name) {
    int iWidth, iHeight, iChans;
    unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return;
    }

    numVertices = iWidth * iHeight;
    numIndices = (iWidth - 1) * (iHeight - 1) * 6;
    vertices = new Vector3[numVertices];
    colours = new Vector4[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector3 vertexScale = Vector3(50.0f, 3.5f, 50.0f);
    Vector2 textureScale = Vector2(1 / 50.0f, 1.0f / 50.0f);

    for (int z = 0; z < iHeight; ++z) {
        for (int x = 0; x < iWidth; ++x) {
            int offset = (z * iWidth) + x;
            vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
            textureCoords[offset] = Vector2(x, z) * textureScale;
            colours[offset] =  Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    SOIL_free_image_data(data);

    int i = 0;

    for (int z = 0; z < iHeight - 1; ++z) {
        for (int x = 0; x < iWidth - 1; ++x) {
            int a = (z * iWidth) + x;
            int b = (z * iWidth) + (x + 1);
            int c = ((z + 1) * iWidth) + (x + 1);
            int d = ((z + 1) * iWidth) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }

    GenerateNormals();
    GenerateTangents();
    BufferData();

    heightmapSize.x = vertexScale.x * (iWidth - 1);
    heightmapSize.y = vertexScale.y * 255.0f; // each height is a byte!
    heightmapSize.z = vertexScale.z * (iHeight - 1);
}

HeightMap::~HeightMap() {
}

Vector3 HeightMap::GetWorldCoordinatesFromTextureCoords(float u, float v) {

    int iWidth, iHeight, iChans;
    Vector3 vertexScale = Vector3(50.0f, 3.5f, 50.0f);
    Vector2 textureScale = Vector2(1 / 5000.0f, 1.0f / 5000.0f);

    unsigned char* data = SOIL_load_image(TEXTUREDIR "valleyTex.png", &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return Vector3(0, 0, 0);
    }

    // First, we need to map (u, v) to the terrain grid
    int x = static_cast<int>(u * (heightmapSize.x / vertexScale.x)); // Get the x grid index
    int z = static_cast<int>(v * (heightmapSize.z / vertexScale.z)); // Get the z grid index

    // Fetch the height value at (x, z)
    int index = (z * iWidth) + x;
    float height = vertices[index].y;  // This stores the height value (y coordinate)

    // The world-space position of the point is calculated using the vertex scale.
    // We apply the scaling that was used when creating the terrain.
    Vector3 worldPosition = Vector3(x, height, z) * vertexScale;

    return worldPosition;
}
