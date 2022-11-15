//
// Created by elise on 26/09/2022.
//
#pragma once

#include "Mesh.h"
#include "Material.h"

class Renderable {
public:
    Renderable() = delete;
    Renderable &operator=(const Renderable&) = delete;
    Renderable(const Mesh& pMesh, const Material& pMaterial);
    ~Renderable() = default;

    const Material* getMaterial() const {return &material;}
    const Mesh* getMesh() const {return &mesh;}

private:
    const std::string meshName;
    const Mesh& mesh;
    const Material& material;

    void load();
};
