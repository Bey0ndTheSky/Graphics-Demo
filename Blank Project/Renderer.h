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

protected:
    SceneNode* root;
    HeightMap* heightMap;
    std::vector<Shader*> shaderVec;
   
    Camera* camera;
    GLuint terrainTex;
    GLuint waterTex;
    GLuint dispTex;
    GLuint windTex;
    GLuint cubeMap;

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
};
