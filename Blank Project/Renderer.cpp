#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    quad = Mesh::GenerateQuad();

    heightMap = new HeightMap(TEXTUREDIR "valleyTex.png");
    heightMap->SetPrimitiveType(GL_PATCHES);
    camera = new Camera(-40, 270, Vector3());

    Vector3 dimensions = heightMap->GetHeightmapSize();
    camera->SetPosition(dimensions * Vector3(0.5, 1, 0.5));


    SetShaders();
    SetTextures();

    projMatrix = Matrix4::Perspective(1.0f, 80000.0f, (float)width / (float)height, 45.0f);

    glEnable(GL_DEPTH_TEST);
    glClipControl(GL_LOWER_LEFT,
        GL_ZERO_TO_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    waterRotate = 0.0f;
    waterCycle = 0.0f;
    windTranslate = 0.0f;
    windStrength = 0.5f;

    currentFrame = 0;
    frameTime = 0.0f;

    root = new SceneNode();


    SetMeshes();

    init = true;
}

Renderer::~Renderer(void) {
    delete heightMap;
    delete camera;
    delete root;
    delete quad;
    delete anim;

    for (Shader* shader : shaderVec) {
        delete shader;
    }

}

void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();

    frameTime -= dt;
    while (frameTime < 0.0f) {
        currentFrame = (currentFrame + 1) % anim->GetFrameCount();
        frameTime += 1.0f / anim->GetFrameRate();
    }
    
    waterRotate += dt * 0.1f;
    waterCycle += dt * 0.05f;

    windTranslate += dt * (0.015f + cos(dt * 0.01f) * 0.01f);
    windStrength = 0.3f * sin(dt * 0.05f) * 0.29 ;

    frameFrustum.FromMatrix(projMatrix * viewMatrix);
    root->Update(dt);
}

void Renderer::RenderScene() {
    BuildNodeLists(root);
    SortNodeLists();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 2, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    DrawGround();

    DrawNodes();
    ClearNodeLists();

    DrawWater();

    DrawSkybox();
    glDisable(GL_STENCIL_TEST);

}

void Renderer::DrawGround() {
    BindShader(shaderVec[GROUND_SHADER]);
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTex);

    glUniform1i(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "DisplacementMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dispTex);

    glUniform1i(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "windMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, windTex);

    glUniform1f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "dispFactor"), 0.1f);
    glUniform1f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "grassHeight"), 25.0f);
    glUniform1f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "bladeWidth"), 5.0f);

    glUniform1f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "windTraslate"), windTranslate);
    glUniform1f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "windStrength"), windStrength);

    glUniform3fv(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "cameraPosition"), 1, (float*)&camera->GetPosition());
    glUniform4f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "colourBase"), 0.0f, 0.8f, 0.0f, 1.0f);  // Green
    glUniform4f(glGetUniformLocation(shaderVec[GROUND_SHADER]->GetProgram(), "colourTop"), 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow

    modelMatrix.ToIdentity();
    textureMatrix.ToIdentity();

    UpdateShaderMatrices();

    heightMap->Draw();
}

void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);

    glStencilFunc(GL_EQUAL, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    BindShader(shaderVec[SKYBOX_SHADER]);
    UpdateShaderMatrices();
    quad->Draw();

    glDepthMask(GL_TRUE);
}

void Renderer::DrawWater() {
    BindShader(shaderVec[REFLECT_SHADER]);

    glUniform3fv(glGetUniformLocation(shaderVec[REFLECT_SHADER]->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shaderVec[REFLECT_SHADER]->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(shaderVec[REFLECT_SHADER]->GetProgram(), "cubeTex"), 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    Vector3 hSize = heightMap->GetHeightmapSize();

    modelMatrix =
        Matrix4::Translation((hSize - Vector3(0.0f, 2.4f * 255.0f, 0.0f))* 0.52f) *
        Matrix4::Scale(hSize * 0.52f) *
        Matrix4::Rotation(90, Vector3(-1, 0, 0));

    textureMatrix =
        Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
        Matrix4::Scale(Vector3(10, 10, 10)) *
        Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

    UpdateShaderMatrices();
    // SetShaderLight(*light); // No lighting in this shader!

    quad->Draw();

}

void Renderer::DrawNode(SceneNode* n) {
    if (n->GetAnim() && n->GetMesh()) { DrawAnim(n); }

    else if (n->GetMesh()) {
        BindShader(shaderVec[SCENE_SHADER]);

        modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
        UpdateShaderMatrices();

        glUniform1i(glGetUniformLocation(shaderVec[SCENE_SHADER]->GetProgram(), "diffuseTex"), 0);

        for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, n->matTextures[i]);
            n->GetMesh()->DrawSubMesh(i);
        }

        //n->Draw(*this);
    }
}

void Renderer::DrawAnim(SceneNode* n) {
    BindShader(shaderVec[SKINNING_SHADER]);

    modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(shaderVec[SKINNING_SHADER]->GetProgram(), "diffuseTex"), 0);

    std::vector<Matrix4> frameMatrices;

    const Matrix4* invBindPose = n->GetMesh()->GetInverseBindPose();
    const Matrix4* frameData = n->GetAnim()->GetJointData(currentFrame);

    for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); ++i) {
        frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    }

    int j = glGetUniformLocation(shaderVec[SKINNING_SHADER]->GetProgram(), "joints");
    glUniformMatrix4fv(j, frameMatrices.size(), GL_FALSE, (float*)frameMatrices.data());

    for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, n->matTextures[i]);
        n->GetMesh()->DrawSubMesh(i);
    }
}

void Renderer::SetShaders() {

    shaderVec = {
    new Shader("HeightmapVertex.glsl", "HeightmapFragment.glsl", "heightmapGeometry.glsl", "groundTCS.glsl", "groundTES.glsl"),
    new Shader("skyboxVertex.glsl", "skyboxFragment.glsl"),
    new Shader("reflectVertex.glsl", "reflectFragment.glsl"),
    new Shader("TexturedColouredVertex.glsl", "TexturedColouredFragment.glsl"),
    new Shader("SkinningVertex.glsl", "TexturedFragment.glsl")
    };

    for (Shader* shader : shaderVec) {
        if (!shader->LoadSuccess()) {
            std::cerr << "Shader loading failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

void Renderer::SetTextures() {

    terrainTex = SOIL_load_OGL_texture(TEXTUREDIR "Grass_lighted_down.PNG",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS);

    dispTex = SOIL_load_OGL_texture(TEXTUREDIR "grass_displacement.PNG",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS);

    waterTex = SOIL_load_OGL_texture(
        TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    windTex = SOIL_load_OGL_texture(
        TEXTUREDIR "wind.png", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    cubeMap = SOIL_load_OGL_cubemap(
        TEXTUREDIR "Epic_GloriousPink_Cam_2_Left+X.png", TEXTUREDIR "Epic_GloriousPink_Cam_3_Right-X.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_4_Up+Y.png", TEXTUREDIR "Epic_GloriousPink_Cam_5_Down-Y.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_0_Front+Z.png", TEXTUREDIR "Epic_GloriousPink_Cam_1_Back-Z.png",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
    );
    if (!terrainTex || !cubeMap || !dispTex || !waterTex) {
        return;
    }

    SetTextureRepeating(terrainTex, true);
    SetTextureRepeating(waterTex, true);
    SetTextureRepeating(windTex, true);

}

void Renderer::SetMeshes() {

    mesh = Mesh::LoadFromMeshFile("Role_T.msh");
    anim = new MeshAnimation("Role_T.anm");
    material = new MeshMaterial("Role_T.mat");

    SceneNode* guy = new SceneNode();
    guy->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.5, 0.5, 0.5)));
    guy->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
    guy->SetBoundingRadius(100.0f);
    guy->SetMesh(mesh);
    guy->SetAnim(anim);
    guy->SetMaterial(material);
    //root->AddChild(guy);

    for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);

        string path = TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        guy->matTextures.emplace_back(texID);
    }

    mesh = Mesh::LoadFromMeshFile("new/persona_4_-_television.prefab.msh");
    material = new MeshMaterial("new/persona_4_-_television.prefab.mat");

    SceneNode* s = new SceneNode();
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.5, 2.0, 0.5)));
    s->SetModelScale(Vector3(10.0f, 10.0f, 10.0f));
    s->SetBoundingRadius(100.0f);
    s->SetMesh(mesh);
    s->SetMaterial(material);
    root->AddChild(s);

    for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);

        string path = TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        s->matTextures.emplace_back(texID);
    }
}

void Renderer::BuildNodeLists(SceneNode* from) {
    if (frameFrustum.InsideFrustum(*from)) {
        Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
        from->SetCameraDistance(Vector3::Dot(dir, dir));

        if (from->GetColour().w < 1.0f) {
            transparentNodeList.push_back(from);
        }
        else {
            nodeList.push_back(from);
        }
    }

    for (std::vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
        i != from->GetChildIteratorEnd(); ++i) {
        BuildNodeLists(*i);
    }
}

void Renderer::SortNodeLists() {
    std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(),
        SceneNode::CompareByCameraDistance);
    std::sort(nodeList.begin(), nodeList.end(),
        SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
    for (const auto& i : nodeList) {
        DrawNode(i);
    }
    for (const auto& i : transparentNodeList) {
        DrawNode(i);
    }
}

void Renderer::ClearNodeLists() {
    transparentNodeList.clear();
    nodeList.clear();
}