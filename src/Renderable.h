//
// Created by elise on 26/09/2022.
//

#ifndef SHADEREDITOR_RENDERABLE_H
#define SHADEREDITOR_RENDERABLE_H
#include "Mesh.h"
#include "Material.h"

class Renderable {
public:
    Renderable() = default;
    Renderable(Mesh* pMesh, Material* pMaterial);
    ~Renderable() = default;

    Mesh* mesh = nullptr;
    Material* material = nullptr;

    void load();
};


#endif //SHADEREDITOR_RENDERABLE_H
