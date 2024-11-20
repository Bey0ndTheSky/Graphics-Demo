#include "SceneNode.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"

SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
    this->mesh = std::shared_ptr<Mesh>(mesh);
    this->colour = colour;
    parent = nullptr;
    boundingRadius = 1.0f;
    distanceFromCamera = 0.0f;
    texture = 0;
    modelScale = Vector3(1, 1, 1);
    modelRotation = Matrix4::Rotation(0, Vector3(0, 0, 0));
}

SceneNode::~SceneNode() {
    for (unsigned int i = 0; i < children.size(); ++i) {
        delete children[i];
    }
}

void SceneNode::AddChild(SceneNode* s) {
    children.push_back(s);
    s->parent = this;
}

void SceneNode::Draw(const OGLRenderer& r) {
    if (mesh) {
        mesh->Draw();
    }
}

void SceneNode::Update(float dt) {
    if (parent) { // This node has a parent
        worldTransform = parent->worldTransform * transform;
    }
    else { // Root node, world transform is local transform
        worldTransform = transform;
    }

    for (auto i = children.begin(); i != children.end(); ++i) {
        (*i)->Update(dt);
    }
}

void SceneNode::SetMaterial(std::shared_ptr<MeshMaterial> m, bool l) {
    material = m;
    if (l) {
        for (int i = 0; i < material->materialLayers.size(); ++i) {
            MeshMaterialEntry* matEntry = &material->materialLayers[i];
            for (const auto& entry : matEntry->entries) {
                const std::string& textureName = entry.first;
                const std::string& filename = entry.second;

                std::string texturePath = TEXTUREDIR + filename;
                GLuint texID = SOIL_load_OGL_texture(texturePath.c_str(), SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

                if (texID != 0) {
                    matEntry->textures[textureName] = texID;
                }
                else {
                    std::cerr << "Failed to load texture: " << texturePath << std::endl;
                }
            }
        }
    }
}
