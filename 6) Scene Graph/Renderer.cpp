#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
    camera = new Camera();

    shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

    if (!shader->LoadSuccess()) {
        return;
    }

    projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
        static_cast<float>(width) / static_cast<float>(height), 45.0f);

    camera->SetPosition(Vector3(0, 30, 175));

    root = new SceneNode();
    root->AddChild(new CubeRobot(cube));

    glEnable(GL_DEPTH_TEST);
    init = true;
}

Renderer::~Renderer() {
    delete root;
    delete shader;
    delete camera;
    delete cube;
}

void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();
    root->Update(dt);
}

void Renderer::RenderScene() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    BindShader(shader);
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 1);

    DrawNode(root);
}

void Renderer::DrawNode(SceneNode* n) {
    if (n->GetMesh()) {
        Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

        glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, GL_FALSE, model.values);
        glUniform4fv(glGetUniformLocation(shader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());
        glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"), 0); // Next tutorial ;)

        n->Draw(*this);
    }

    for (auto i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
        DrawNode(*i);
    }
}

