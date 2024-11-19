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
    camera->SetPosition(dimensions * Vector3(0.2, 1, 0.2));


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

    root1 = new SceneNode();
    root2 = new SceneNode();


    SetMeshes();

    init = true;
}

Renderer::~Renderer(void) {
    delete heightMap;
    delete camera;
    delete root1;
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
    root1->Update(dt);
}

void Renderer::RenderScene() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 2, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    if (activeScene) {
        BuildNodeLists(activeScene ? root1 : root2);
        SortNodeLists();

        DrawGround();

        
        shader = shaderVec[SCENE_SHADER];
        BindShader(shaderVec[SCENE_SHADER]);
        DrawNodes();
        ClearNodeLists();

        DrawWater();
    }
    else {
        BuildNodeLists(root2);
        SortNodeLists();

        BindShader(shaderVec[SCENE_SHADER]);
        DrawGround();
        DrawNodes();
        ClearNodeLists();

        DrawWater();
    }
    

    DrawSkybox();
    glDisable(GL_STENCIL_TEST);

}

void Renderer::DrawGround() {

    if (activeScene) {
        shader = shaderVec[GROUND_SHADER];
        BindShader(shader);
        
        glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, terrainTex);

        glUniform1i(glGetUniformLocation(shader->GetProgram(), "DisplacementMap"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dispTex);

        glUniform1i(glGetUniformLocation(shader->GetProgram(), "windMap"), 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, windTex);

        glUniform1f(glGetUniformLocation(shader->GetProgram(), "dispFactor"), 0.1f);
        glUniform1f(glGetUniformLocation(shader->GetProgram(), "grassHeight"), 25.0f);
        glUniform1f(glGetUniformLocation(shader->GetProgram(), "bladeWidth"), 5.0f);

        glUniform1f(glGetUniformLocation(shader->GetProgram(), "windTraslate"), windTranslate);
        glUniform1f(glGetUniformLocation(shader->GetProgram(), "windStrength"), windStrength);

        glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPosition"), 1, (float*)&camera->GetPosition());
        glUniform4f(glGetUniformLocation(shader->GetProgram(), "colourBase"), 0.0f, 0.8f, 0.0f, 1.0f);  // Green
        glUniform4f(glGetUniformLocation(shader->GetProgram(), "colourTop"), 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
    }

    else {


    }

    modelMatrix.ToIdentity();
    textureMatrix.ToIdentity();

    UpdateShaderMatrices();
    heightMap->Draw();
}

void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);

    glStencilFunc(GL_EQUAL, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    shader = shaderVec[SKYBOX_SHADER];
    BindShader(shader);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, activeScene ? cubeMap1 : cubeMap2);
    UpdateShaderMatrices();
    quad->Draw();

    glDepthMask(GL_TRUE);
}

void Renderer::DrawWater() {
    shader = shaderVec[REFLECT_SHADER];
    BindShader(shader);

    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, activeScene ? cubeMap1 : cubeMap2);

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

    if (!n->GetMesh()) { return; }

    if (shader != shaderVec[n->GetShader()]) {
        shader = shaderVec[n->GetShader()];
        BindShader(shader);
    }
    if (shader == shaderVec[SKINNING_SHADER]){ DrawAnim(n); }
    else if (shader == shaderVec[REFLECT_SHADER]) { DrawReflect(n); }
    else{
        modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()) * n->GetRotation();
        UpdateShaderMatrices();

        glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
        glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

        if (n->GetMaterial()) {
            for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, (*n->matTextures)[i]);
                n->GetMesh()->DrawSubMesh(i);
            }
        }
        else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, n->GetTexture());
            for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
                n->GetMesh()->DrawSubMesh(i);
            }
        }
        
    }

}

void Renderer::DrawAnim(SceneNode* n) {
    modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()) * n->GetRotation();
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
        glBindTexture(GL_TEXTURE_2D, (*n->matTextures)[i]);
        n->GetMesh()->DrawSubMesh(i);
    }
}

void Renderer::DrawReflect(SceneNode* n) {
    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "useIce"), 0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, n->GetTexture());

    modelMatrix = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()) * n->GetRotation();
    UpdateShaderMatrices();

    if (n->GetMaterial()) {
        for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, (*n->matTextures)[i]);
            n->GetMesh()->DrawSubMesh(i);
        }
    }
    

    Vector3 hSize = heightMap->GetHeightmapSize();

    UpdateShaderMatrices();

}

void Renderer::SetShaders() {

    shaderVec = {
    new Shader("HeightmapVertex.glsl", "HeightmapFragment.glsl", "heightmapGeometry.glsl", "groundTCS.glsl", "groundTES.glsl"),
    new Shader("skyboxVertex.glsl", "skyboxFragment.glsl"),
    new Shader("reflectVertex.glsl", "reflectFragment.glsl"),
    new Shader("TexturedColouredVertex.glsl", "TexturedColouredFragment.glsl"),
    new Shader("TexturedColouredVertexInstanced.glsl", "TexturedColouredFragment.glsl"),
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
        TEXTUREDIR "water.png", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    windTex = SOIL_load_OGL_texture(
        TEXTUREDIR "wind.png", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );
    cubeMap1 = SOIL_load_OGL_cubemap(
        TEXTUREDIR "Epic_GloriousPink_Cam_2_Left+X.png", TEXTUREDIR "Epic_GloriousPink_Cam_3_Right-X.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_4_Up+Y.png", TEXTUREDIR "Epic_GloriousPink_Cam_5_Down-Y.png",
        TEXTUREDIR "Epic_GloriousPink_Cam_0_Front+Z.png", TEXTUREDIR "Epic_GloriousPink_Cam_1_Back-Z.png",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
    );
    cubeMap2 = SOIL_load_OGL_cubemap(
        TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_2_Left+X.png", TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_3_Right-X.png",
        TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_4_Up+Y.png", TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_5_Down-Y.png",
        TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_0_Front+Z.png", TEXTUREDIR "Sky_AllSky_Overcast4_Low_Cam_1_Back-Z.png",
        SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
    );

    if (!terrainTex || !cubeMap1 || !dispTex || !waterTex) {
        return;
    }

    SetTextureRepeating(terrainTex, true);
    SetTextureRepeating(waterTex, true);
    SetTextureRepeating(windTex, true);
}

void Renderer::SetMeshes() {
    anim = new MeshAnimation("Role_T.anm");

    SceneNode* s = loadMeshAndMaterial("Role_T.msh", "Role_T.mat");
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.5, 0.5, 0.5)));
    s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
    s->SetBoundingRadius(100.0f);
    s->SetMesh(mesh);
    s->SetAnim(anim);
    s->SetShader(SKINNING_SHADER);
    root2->AddChild(s);

    std::shared_ptr<Mesh> sharedMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("new/persona_4_-_television.prefab.msh"));
    material = new MeshMaterial("new/persona_4_-_television.prefab.mat");
    
    s = new SceneNode();
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.45, 0.82, 0.45)));
    s->SetModelScale(Vector3(2.0f, 2.0f, 2.0f));
    s->SetBoundingRadius(150.0f);
    s->SetMesh(sharedMesh);
    s->SetShader(SCENE_SHADER);
    s->SetMaterial(material, true);
    root1->AddChild(s);

    sharedMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("new/persona_4_-_television.prefab.msh"));
    material = new MeshMaterial("new/persona_4_-_television.prefab_trans.mat");
    s = new SceneNode();
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.3, 1.0, 0.24)));
    s->SetModelScale(Vector3(4.0f, 4.0f, 4.0f));
    s->SetRotation(Matrix4::Rotation(-90.0f, Vector3(0, 1, 0)));
    s->SetBoundingRadius(300.0f);
    s->SetMesh(sharedMesh);
    s->SetShader(REFLECT_SHADER);
    s->SetMaterial(material, true);
    s->SetTexture(cubeMap1);
    root1->AddChild(s);

    material = new MeshMaterial("new/persona_4_-_television.prefab_trans.mat");
    std::shared_ptr<std::vector<GLuint>> loadedTex = s->matTextures;
    s = new SceneNode();
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.84, 1.7, 0.15)));
    s->SetModelScale(Vector3(10.0f, 10.0f, 10.0f));
    s->SetRotation(Matrix4::Rotation(-100.0f, Vector3(0, 1, 0)));
    s->SetBoundingRadius(750.0f);
    s->SetMesh(sharedMesh);
    s->SetShader(REFLECT_SHADER);
    s->SetMaterial(material);
    s->SetTexture(cubeMap2);
    s->matTextures = loadedTex;
    root1->AddChild(s);

    mesh = Mesh::LoadFromMeshFile("new/lunar_tear.msh");
    s = new SceneNode();
    root1->AddChild(s);
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.725f, 0.28, 0.225f)));
    s->SetBoundingRadius(1000.0f);
    s->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
    s->SetRotation(Matrix4::Rotation(-60.0f, Vector3(0, 1, 0)));
    s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    s->SetShader(SCENE_INSTANCED_SHADER);
    mesh->SetInstances(flowerPos, 100);
    s->SetMesh(mesh);

    s = loadMeshAndMaterial("Sphere.msh", "");
    root2->AddChild(s);
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.728f, 0.45, 0.2f)));
    s->SetBoundingRadius(100.0f);
    s->SetModelScale(Vector3(2000.0f, 2000.0f, 2000.0f));
    s->SetRotation(Matrix4::Rotation(-60.0f, Vector3(0, 1, 0)));
    s->SetShader(SCENE_INSTANCED_SHADER);
    s->GetMesh()->SetInstances(new Vector3[2]{ Vector3(0, 10, 10), Vector3(0,1, 10)}, 2);

    s = loadMeshAndMaterial("new/door (4).msh", "new/door (4).mat");
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.5, 0.8, 0.95)));
    s->SetModelScale(Vector3(3.0f, 3.0f, 3.0f));
    s->SetShader(SCENE_SHADER);
    s->SetBoundingRadius(1300.0f);
    root1->AddChild(s);

    s = loadMeshAndMaterial("new/door (1).msh", "new/door (1).mat");
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.95, 1.71, 0.65)));
    s->SetRotation(Matrix4::Rotation(-90.0f, Vector3(0, 1, 0)));
    s->SetModelScale(Vector3(3.0f, 3.0f, 3.0f));
    s->SetBoundingRadius(250.0f);
    s->SetShader(SCENE_SHADER);
    root1->AddChild(s);

    anim = new MeshAnimation("new/terminal_nier_automata_fan-art.anm");
    s = loadMeshAndMaterial("new/terminal_nier_automata_fan-art.msh", "new/terminal_nier_automata_fan-art.mat");
    s->SetTransform(Matrix4::Translation(heightMap->GetHeightmapSize() * Vector3(0.15, 0.3, 0.1)));
    s->SetRotation(Matrix4::Rotation(150.0f, Vector3(0, 1, 0)));
    s->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
    s->SetBoundingRadius(200.0f);
    s->SetAnim(anim);
    s->SetShader(SKINNING_SHADER);
    root1->AddChild(s);
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
        [](const SceneNode* a, const SceneNode* b) {
            if (a->GetShader() != b->GetShader()) {
                return a->GetShader() < b->GetShader(); 
            }
            return SceneNode::CompareByCameraDistance(a, b);
        });

    std::sort(nodeList.begin(), nodeList.end(),
        [](const SceneNode* a, const SceneNode* b) {
            if (a->GetShader() != b->GetShader()) {
                return a->GetShader() < b->GetShader();  
            }
            return SceneNode::CompareByCameraDistance(a, b);
        });
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

SceneNode* Renderer::loadMeshAndMaterial(const std::string& meshFile, const std::string& materialFile) {
    std::shared_ptr<Mesh> mesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile(meshFile));
    if (!mesh) {
        std::cerr << "Failed to load mesh: " << meshFile << std::endl;
    }

    SceneNode* node = new SceneNode();
    node->SetBoundingRadius(100.0f);
    node->SetMesh(mesh);
    node->SetShader(SCENE_SHADER);

    if (materialFile == "") {  
        std::cerr << "Failed to load material: " << materialFile << std::endl;
    }
    else
    {
        MeshMaterial* material = new MeshMaterial(materialFile);
        node->SetMaterial(material, true);
    }
    return node;
}
