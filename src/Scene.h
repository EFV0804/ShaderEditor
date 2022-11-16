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
    void draw();
    void cleanUp();
    DeletionQueue sceneDeletionQueue;

private:
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    glm::vec3 camPos{0.f,0.f,-2.f};
    glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);

};