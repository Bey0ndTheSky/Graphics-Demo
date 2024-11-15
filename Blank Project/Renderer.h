#pragma once
#include "../nclgl/OGLRenderer.h"
# include "../nclgl/Frustum.h"
# include "../nclgl/SceneNode.h"


class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);

    void RenderScene() override;
    void UpdateScene(float dt) override;
    void DrawGround();
    void DrawSkybox();
    void DrawWater();

protected:
    SceneNode* root;
    HeightMap* heightMap;
    Shader* groundShader;
    Shader* skyboxShader;
    Shader* reflectShader;
    Camera* camera;
    GLuint terrainTex;
    GLuint waterTex;
    GLuint dispTex;
    GLuint windTex;
    GLuint cubeMap;

    Frustum frameFrustum;

    Mesh* quad;

    float waterRotate;
    float waterCycle;

    float windTranslate;
    float windStrength;
};
