//
// Created by elise on 26/09/2022.
//
#pragma once

#include <vector>
#include "Renderable.h"
#include "Renderer.h"
#include "glm/gtx/transform.hpp"



class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    ~Scene();

    std::vector<Renderable> renderables;
    void load();
    void update(float dt);
    void draw();
    void cleanUp();
    DeletionQueue sceneDeletionQueue;

    CameraBuffer camBuffer;
    glm::vec3 camPos{-18.0f, -17.0f, -25.0f};

private:
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
};