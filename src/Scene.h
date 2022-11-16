//
// Created by elise on 26/09/2022.
//
#pragma once

#include <vector>
#include "Renderable.h"
#include "Renderer.h"


class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    std::vector<Renderable> renderables;
    void load();
    void draw();
    void cleanUp();
    DeletionQueue sceneDeletionQueue;

private:
    std::vector<Material> materials;
    std::vector<Mesh> meshes;

};