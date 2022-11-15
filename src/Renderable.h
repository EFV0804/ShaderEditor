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
    Renderable(Mesh* pMesh, Material* pMaterial);
    ~Renderable() = default;

    Material* getMaterial() const {return material;}
    Mesh* getMesh() const {return mesh;}

private:
    const std::string meshName;
    Mesh* mesh;
    Material* material;

    void load();
};
