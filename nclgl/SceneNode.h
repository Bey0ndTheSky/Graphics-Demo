#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

class MeshAnimation;
class MeshMaterial;

class SceneNode {
public:
    SceneNode(Mesh* m = nullptr, Vector4 colour = Vector4(0, 0, 0, 1));
    //SceneNode(std::shared_ptr<Mesh>, Vector4 colour = Vector4(1, 1, 1, 1));
    ~SceneNode();

    void SetTransform(const Matrix4& matrix) { transform = matrix; }
    const Matrix4& GetTransform() const { return transform; }
    Matrix4 GetWorldTransform() const { return worldTransform; }

    void SetRotation(const Matrix4& matrix) { modelRotation = matrix; }
    const Matrix4& GetRotation() const { return modelRotation; }

    Vector4 GetColour() const { return colour; }
    void SetColour(Vector4 c) { colour = c; }

    int GetShader() const { return shader; }
    void SetShader(int s) { shader = s; }

    Vector3 GetModelScale() const { return modelScale; }
    void SetModelScale(Vector3 s) { modelScale = s; }

    Mesh* GetMesh() const { return mesh.get(); }
    void SetMesh(Mesh* m) { mesh = std::shared_ptr<Mesh>(m); }
    void SetMesh(std::shared_ptr<Mesh> m) { mesh = m; }

    MeshAnimation* GetAnim() const { return anim; }
    void SetAnim(MeshAnimation* a) { anim = a; }
    
    MeshMaterial* GetMaterial() const { return material.get(); }
    void SetMaterial(MeshMaterial* m, bool l = false) { SetMaterial(std::shared_ptr<MeshMaterial>(m), l); }
    void SetMaterial(std::shared_ptr<MeshMaterial> m, bool l = false);

    float GetBoundingRadius() const { return boundingRadius; }
    void SetBoundingRadius(float f) { boundingRadius = f; }

    float GetCameraDistance() const { return distanceFromCamera; }
    void SetCameraDistance(float f) { distanceFromCamera = f; }

    void SetTexture(GLuint tex) { texture = tex; }
    GLuint GetTexture() const { return texture; }

    static const bool CompareByCameraDistance(const SceneNode* a, const SceneNode* b) {
        return a->distanceFromCamera < b->distanceFromCamera;
    }

    void AddChild(SceneNode* s);
    virtual void Update(float dt);
    virtual void Draw(const OGLRenderer& r);

    std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
        return children.begin();
    }

    std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
        return children.end();
    }

protected:
    SceneNode* parent;
    std::shared_ptr<Mesh> mesh;
    int shader;
    Matrix4 worldTransform;
    Matrix4 transform;
    Vector3 modelScale;
    Matrix4 modelRotation;
    Vector4 colour;
    float distanceFromCamera;
    float boundingRadius;
    GLuint texture;

    MeshAnimation* anim = nullptr;
    std::shared_ptr<MeshMaterial> material;
    std::vector<SceneNode*> children;
};
