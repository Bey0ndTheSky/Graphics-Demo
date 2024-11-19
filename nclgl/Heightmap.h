#pragma once

#include <string>
#include "mesh.h"

class HeightMap : public Mesh {
public:
    HeightMap(const std::string& name);
    ~HeightMap();

    Vector3 GetHeightmapSize() const { return heightmapSize; }
    Vector3 GetWorldCoordinatesFromTextureCoords(float u, float v);

protected:
    Vector3 heightmapSize;
};
