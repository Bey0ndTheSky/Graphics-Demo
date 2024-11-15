#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    quad = Mesh::GenerateQuad();

    heightMap = new HeightMap(TEXTUREDIR "valleyTex.png");
    heightMap->SetPrimitiveType(GL_PATCHES);
    camera = new Camera(-40, 270, Vector3());

    Vector3 dimensions = heightMap->GetHeightmapSize();
    camera->SetPosition(dimensions * Vector3(0.5, 1, 0.5));

    groundShader = new Shader("HeightmapVertex.glsl", "HeightmapFragment.glsl", "heightmapGeometry.glsl",
        "groundTCS.glsl", "groundTES.glsl"
    );
    skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
    
    reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");

    if (!groundShader->LoadSuccess() || !skyboxShader->LoadSuccess()) {
        return;
    }

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

    cubeMap = SOIL_load_OGL_cubemap(
        TEXTUREDIR "Epic_GloriousPink_Cam_2_Left+X.png", TEXTUREDIR "Epic_GloriousPink_Cam_3_Right-X.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_4_Up+Y.png", TEXTUREDIR "Epic_GloriousPink_Cam_5_Down-Y.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_0_Front+Z.png",TEXTUREDIR "Epic_GloriousPink_Cam_1_Back-Z.png",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
    );
    if (!terrainTex || !cubeMap || !dispTex || !waterTex) {
        return;
    }

    SetTextureRepeating(terrainTex, true);
    SetTextureRepeating(waterTex, true);

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
    init = true;
}

Renderer::~Renderer(void) {
    delete heightMap;
    delete camera;
    delete groundShader;
    delete skyboxShader;
    //delete root;
    delete quad;
    delete reflectShader;

}

void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();
    waterRotate += dt * 1.0f;
    waterCycle += dt * 0.05f;
    frameFrustum.FromMatrix(projMatrix * viewMatrix);
}

void Renderer::RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 2, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    DrawGround();
    DrawWater();
    DrawSkybox();

    glDisable(GL_STENCIL_TEST);

}

void Renderer::DrawGround() {
    BindShader(groundShader);
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTex);

    glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "DisplacementMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dispTex);

    glUniform1f(glGetUniformLocation(groundShader->GetProgram(), "dispFactor"), 0.1f);
    glUniform1f(glGetUniformLocation(groundShader->GetProgram(), "grassHeight"), 20.0f);
    glUniform1f(glGetUniformLocation(groundShader->GetProgram(), "bladeWidth"), 5.0f);

    glUniform3fv(glGetUniformLocation(groundShader->GetProgram(), "cameraPosition"), 1, (float*)&camera->GetPosition());
    glUniform4f(glGetUniformLocation(groundShader->GetProgram(), "colorBase"), 0.0f, 0.8f, 0.0f, 1.0f);  // Green
    glUniform4f(glGetUniformLocation(groundShader->GetProgram(), "colorTop"), 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow

    modelMatrix.ToIdentity();
    textureMatrix.ToIdentity();

    UpdateShaderMatrices();

    heightMap->Draw();
}

void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);

    glStencilFunc(GL_EQUAL, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    BindShader(skyboxShader);
    UpdateShaderMatrices();
    quad->Draw();

    glDepthMask(GL_TRUE);
}

void Renderer::DrawWater() {
    BindShader(reflectShader);

    glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    Vector3 hSize = heightMap->GetHeightmapSize();

    modelMatrix =
        Matrix4::Translation((hSize - Vector3(0.0f, 2.4f * 255.0f, 0.0f))* 0.5f) *
        Matrix4::Scale(hSize * 0.5f) *
        Matrix4::Rotation(90, Vector3(-1, 0, 0));

    textureMatrix =
        Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
        Matrix4::Scale(Vector3(10, 10, 10)) *
        Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));

    UpdateShaderMatrices();
    // SetShaderLight(*light); // No lighting in this shader!

    quad->Draw();

}