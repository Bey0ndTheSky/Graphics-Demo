#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/SceneNode.h"
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
    void DrawAnim(SceneNode* n);
    void SetTextures();
    void SetShaders();
    void SetMeshes();

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
    GLuint cubeMap1;
    GLuint cubeMap2;

    Frustum frameFrustum;
    std::vector<SceneNode*> transparentNodeList;
    std::vector<SceneNode*> nodeList;

    Mesh* quad;
    Mesh* mesh;

    MeshAnimation* anim;
    MeshMaterial* material;

    int currentFrame;
    float frameTime;

    float waterRotate;
    float waterCycle;

    float windTranslate;
    float windStrength;

    Vector3 flowerPos[20] = {
    Vector3(0.01f, 0.22f, 0.01f),
    Vector3(0.02f, 0.22f, 0.02f),
    Vector3(0.03f, 0.22f, 0.03f),
    Vector3(0.04f, 0.22f, 0.04f),
    Vector3(0.05f, 0.22f, 0.05f),
    Vector3(0.06f, 0.22f, 0.06f),
    Vector3(0.07f, 0.22f, 0.07f),
    Vector3(0.08f, 0.22f, 0.08f),
    Vector3(0.09f, 0.22f, 0.09f),
    Vector3(0.10f, 0.22f, 0.10f),
    Vector3(0.11f, 0.22f, 0.11f),
    Vector3(0.12f, 0.22f, 0.12f),
    Vector3(0.13f, 0.22f, 0.13f),
    Vector3(0.14f, 0.22f, 0.14f),
    Vector3(0.15f, 0.22f, 0.15f),
    Vector3(0.16f, 0.22f, 0.16f),
    Vector3(0.17f, 0.22f, 0.17f),
    Vector3(0.18f, 0.22f, 0.18f),
    Vector3(0.19f, 0.22f, 0.19f),
    Vector3(0.20f, 0.22f, 0.20f)
    };

};
