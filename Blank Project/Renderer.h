#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/light.h"
#include <memory>

class Mesh;
class MeshAnimation;
class MeshMaterial;
class HeightMap;
class Camera;

enum ShaderIndices{
        GROUND_SHADER,
        SKYBOX_SHADER,
        REFLECT_SHADER,
        SCENE_SHADER,
        SCENE_INSTANCED_SHADER,
        SKINNING_SHADER,
        SNOW_SHADER
};

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);

    void RenderScene() override;
    void UpdateScene(float dt) override;
    void DrawGround();
    void DrawSkybox();
    void DrawWater();
    void DrawReflect(SceneNode* n);
    void DrawAnim(SceneNode* n);
    void SetTextures();
    void SetShaders();
    void SetMeshes();
    void changeScene();

    void BuildNodeLists(SceneNode* from);
    void SortNodeLists();
    void ClearNodeLists();
    void DrawNodes();
    void DrawNode(SceneNode* n);

    SceneNode* loadMeshAndMaterial(const std::string& meshFile, const std::string& materialFile = "");

protected:
    SceneNode* root1;
    SceneNode* root2;
    int activeScene = 1;
    HeightMap* heightMap;
    std::vector<Shader*> shaderVec;
    Shader* shader;
   
    Camera* camera;
    GLuint terrainTex;
    GLuint waterTex;
    GLuint dispTex;
    GLuint windTex;
    GLuint snowDiff;
    GLuint snowBump;
    GLuint cubeMap1;
    GLuint cubeMap2;

    Frustum frameFrustum;
    std::vector<SceneNode*> transparentNodeList;
    std::vector<SceneNode*> nodeList;

    Mesh* quad;
    Mesh* mesh;
    Light* light;

    MeshAnimation* anim;
    MeshMaterial* material;

    float lightParam = 0;
    int currentFrame;
    float frameTime;

    float waterRotate;
    float waterCycle;

    float windTranslate;
    float windStrength;

    Vector3 flowerPos[100] = {
        // Radius 10.0f
        Vector3(10.0f, 0.0f, 0.0f),
        Vector3(9.51f, 0.0f, 3.09f),
        Vector3(8.09f, 0.0f, 5.88f),
        Vector3(5.88f, 0.0f, 8.09f),
        Vector3(3.09f, 0.0f, 9.51f),
        Vector3(0.0f, 0.0f, 10.0f),
        Vector3(-3.09f, 0.0f, 9.51f),
        Vector3(-5.88f, 0.0f, 8.09f),
        Vector3(-8.09f, 0.0f, 5.88f),
        Vector3(-9.51f, 0.0f, 3.09f),
        Vector3(-10.0f, 0.0f, 0.0f),
        Vector3(-9.51f, 0.0f, -3.09f),
        Vector3(-8.09f, 0.0f, -5.88f),
        Vector3(-5.88f, 0.0f, -8.09f),
        Vector3(-3.09f, 0.0f, -9.51f),
        Vector3(0.0f, 0.0f, -10.0f),
        Vector3(3.09f, 0.0f, -9.51f),
        Vector3(5.88f, 0.0f, -8.09f),
        Vector3(8.09f, 0.0f, -5.88f),
        Vector3(9.51f, 0.0f, -3.09f),

        // Radius 8.0f
        Vector3(8.0f, 0.0f, 0.0f),
        Vector3(7.64f, 0.0f, 2.76f),
        Vector3(6.28f, 0.0f, 4.95f),
        Vector3(4.95f, 0.0f, 6.28f),
        Vector3(2.76f, 0.0f, 7.64f),
        Vector3(0.0f, 0.0f, 8.0f),
        Vector3(-2.76f, 0.0f, 7.64f),
        Vector3(-4.95f, 0.0f, 6.28f),
        Vector3(-6.28f, 0.0f, 4.95f),
        Vector3(-7.64f, 0.0f, 2.76f),
        Vector3(-8.0f, 0.0f, 0.0f),
        Vector3(-7.64f, 0.0f, -2.76f),
        Vector3(-6.28f, 0.0f, -4.95f),
        Vector3(-4.95f, 0.0f, -6.28f),
        Vector3(-2.76f, 0.0f, -7.64f),
        Vector3(0.0f, 0.0f, -8.0f),
        Vector3(2.76f, 0.0f, -7.64f),
        Vector3(4.95f, 0.0f, -6.28f),
        Vector3(6.28f, 0.0f, -4.95f),
        Vector3(7.64f, 0.0f, -2.76f),

        // Radius 5.0f
        Vector3(5.0f, 0.0f, 0.0f),
        Vector3(4.85f, 0.0f, 1.55f),
        Vector3(4.29f, 0.0f, 3.83f),
        Vector3(3.83f, 0.0f, 4.29f),
        Vector3(1.55f, 0.0f, 4.85f),
        Vector3(0.0f, 0.0f, 5.0f),
        Vector3(-1.55f, 0.0f, 4.85f),
        Vector3(-3.83f, 0.0f, 4.29f),
        Vector3(-4.29f, 0.0f, 3.83f),
        Vector3(-4.85f, 0.0f, 1.55f),
        Vector3(-5.0f, 0.0f, 0.0f),
        Vector3(-4.85f, 0.0f, -1.55f),
        Vector3(-4.29f, 0.0f, -3.83f),
        Vector3(-3.83f, 0.0f, -4.29f),
        Vector3(-1.55f, 0.0f, -4.85f),
        Vector3(0.0f, 0.0f, -5.0f),
        Vector3(1.55f, 0.0f, -4.85f),
        Vector3(3.83f, 0.0f, -4.29f),
        Vector3(4.29f, 0.0f, -3.83f),
        Vector3(4.85f, 0.0f, -1.55f),

        // Radius 3.0f
        Vector3(3.0f, 0.0f, 0.0f),
        Vector3(2.85f, 0.0f, 0.90f),
        Vector3(2.55f, 0.0f, 1.95f),
        Vector3(2.05f, 0.0f, 2.55f),
        Vector3(1.35f, 0.0f, 2.85f),
        Vector3(0.0f, 0.0f, 3.0f),
        Vector3(-1.35f, 0.0f, 2.85f),
        Vector3(-2.05f, 0.0f, 2.55f),
        Vector3(-2.55f, 0.0f, 1.95f),
        Vector3(-2.85f, 0.0f, 0.90f),
        Vector3(-3.0f, 0.0f, 0.0f),
        Vector3(-2.85f, 0.0f, -0.90f),
        Vector3(-2.55f, 0.0f, -1.95f),
        Vector3(-2.05f, 0.0f, -2.55f),
        Vector3(-1.35f, 0.0f, -2.85f),
        Vector3(0.0f, 0.0f, -3.0f),
        Vector3(1.35f, 0.0f, -2.85f),
        Vector3(2.05f, 0.0f, -2.55f),
        Vector3(2.55f, 0.0f, -1.95f),
        Vector3(2.85f, 0.0f, -0.90f),

        // Radius 1.0f
        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(0.98f, 0.0f, 0.18f),
        Vector3(0.92f, 0.0f, 0.39f),
        Vector3(0.82f, 0.0f, 0.58f),
        Vector3(0.68f, 0.0f, 0.76f),
        Vector3(0.5f, 0.0f, 0.87f),
        Vector3(0.31f, 0.0f, 0.92f),
        Vector3(0.11f, 0.0f, 0.98f),
        Vector3(-0.11f, 0.0f, 0.98f),
        Vector3(-0.31f, 0.0f, 0.92f),
        Vector3(-0.5f, 0.0f, 0.87f),
        Vector3(-0.68f, 0.0f, 0.76f),
        Vector3(-0.82f, 0.0f, 0.58f),
        Vector3(-0.92f, 0.0f, 0.39f),
        Vector3(-0.98f, 0.0f, 0.18f),
        Vector3(-1.0f, 0.0f, 0.0f),
        Vector3(-0.98f, 0.0f, -0.18f),
        Vector3(-0.92f, 0.0f, -0.39f),
        Vector3(-0.82f, 0.0f, -0.58f),
        Vector3(-0.68f, 0.0f, -0.76f)
    };

    Vector4 lerp(const Vector4& a, const Vector4& b, float t) {
        return a + (b - a) * t;
    }
};
