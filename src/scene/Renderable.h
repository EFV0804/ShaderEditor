//
// Created by elise on 26/09/2022.
//
#pragma once

#include "Material.h"
#include "glm/glm.hpp"

class Renderable {
public:
    Renderable() = delete;
    Renderable &operator=(const Renderable&) = delete;
    Renderable(const Mesh& pMesh, const Material& pMaterial, glm::mat4 pTransform);
    ~Renderable() = default;

    const Material* getMaterial() const {return &material;}
    const Mesh* getMesh() const {return &mesh;}
    glm::mat4 transform;

private:
    const std::string meshName;
    const Mesh& mesh;
    const Material& material;


    void load();
};
