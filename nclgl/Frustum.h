#pragma once
#include "Plane.h"

class SceneNode; // Forward declaration
class Matrix4;  // Forward declaration

class Frustum {
public:
    Frustum() {}
    ~Frustum() {}

    void FromMatrix(const Matrix4& mvp);
    bool InsideFrustum(SceneNode& n);

protected:
    Plane planes[6];
};
